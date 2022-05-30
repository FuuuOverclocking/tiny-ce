use nix::errno::Errno;
use nix::sys::wait::{waitpid, WaitPidFlag, WaitStatus};
use nix::unistd::Pid;

use crate::container::{
    fork_container, ipc, userns, ContainerConfig, ContainerState, ContainerStatus,
};
use std::fs;
use std::path::{Path, PathBuf};

pub const CONTAINER_ROOT_PATH: &'static str = "/tmp/tiny-ce";

pub fn create(id: &String, bundle: &String) {
    let bundle_path = fs::canonicalize(Path::new(bundle)).unwrap();
    let config_path = bundle_path.join("config.json");
    let config = ContainerConfig::read_to_config(&config_path);

    let container_path_str = format!("{}/{}", CONTAINER_ROOT_PATH, id);
    let container_path = Path::new(&container_path_str);

    let mut state = ContainerState::new(id, bundle);
    println!("容器状态: {:?}", state);
    state.save_to(container_path);

    // 将启动 container 子进程, 准备 2 个 Unix socket 用于与之通信:
    //
    // - {container_path}/init.sock:
    //     - server: runtime 进程, client: container 进程
    //     - 当 container 成功启动, 并开始在 {container_path}/container.sock 上监听时,
    //       container 发送 "ok"
    // - {container_path}/container.sock:
    //     - server: container 进程, client: runtime 进程
    //     - container 将 accept 两次, 第一次发生在 create() 中, 第二次发生在 start() 中
    //     - 在 create() 中:
    //         1. runtime <- container: 若发生错误, 发送 /error:.*/
    //         2. runtime -> container: 完成 uid/gid mapping 的写入后, 发送 "mapped"
    //         3. runtime <- container: 在 pivot_root(2) 之前, 发送 "pivot?"
    //         4. runtime -> container: 收到 "pivot?" 后, 发送 "ok"
    //         5. runtime <- container: 准备就绪, 可以 start 时, 发送 "ready"
    //     - 在 start() 中:
    //         1. runtime -> container: 发送 "start"

    let init_lock_path = format!("{container_path_str}/init.sock");
    let sock_path = format!("{container_path_str}/container.sock");

    let pid = {
        let init_lock = ipc::IpcParent::new(&init_lock_path);

        // 通过 clone(2) 启动子进程
        let pid = fork_container(&config, &config_path, &init_lock_path, &sock_path);

        let msg = init_lock.wait();
        if !msg.eq("ok") {
            panic!("子进程未发送 ok, 发送了 {msg}");
        }
        init_lock.close();
        pid
    };

    let ipc_channel = ipc::IpcChannel::connect(&sock_path);

    if userns::should_setup_mapping(&config) {
        userns::setup_mapping(&config, &pid);
        ipc_channel.send("mapped");
    }

    loop {
        let msg = ipc_channel.recv();
        if msg.starts_with("error") {
            panic!("子进程发生错误: {}", msg);
        } else if msg.eq("pivot?") {
            ipc_channel.send("ok");
        } else if msg.eq("ready") {
            break;
        }
    }

    state.status = ContainerStatus::Created;
    state.pid = Some(pid.as_raw() as usize);
    println!("容器状态: {:?}", state);
    state.save_to(container_path);
}

pub fn start(id: &String) {
    let container_path = Path::new(CONTAINER_ROOT_PATH).join(id);
    let mut state = ContainerState::try_from(container_path.as_path()).unwrap();

    if state.status != ContainerStatus::Created {
        panic!("试图 start 一个状态不是 Created 的容器.");
    }

    let sock_path = format!("{}/container.sock", container_path.display());
    let ipc_channel = ipc::IpcChannel::connect(&sock_path);
    ipc_channel.send(&"start".to_string());
    ipc_channel.close();

    state.status = ContainerStatus::Running;
    println!("容器状态: {:?}", state);
    state.save_to(container_path.as_path());
}

pub fn delete(id: &String) {
    let container_path = Path::new(CONTAINER_ROOT_PATH).join(id);
    let mut state = ContainerState::try_from(container_path.as_path()).unwrap();

    if state.status != ContainerStatus::Running && state.status != ContainerStatus::Stopped {
        panic!("试图 delete 尚未创建或运行的容器.");
    }

    check_stopped(&mut state, &container_path);

    if state.status != ContainerStatus::Stopped {
        panic!("试图 delete 仍在运行的容器.");
    }
    println!("容器状态: {:?}", state);
    if std::fs::remove_dir_all(Path::new(CONTAINER_ROOT_PATH).join(id)).is_err() {
        println!("删除容器失败.");
    }
}

fn check_stopped(state: &mut ContainerState, container_path: &PathBuf) {
    match waitpid(
        Pid::from_raw(state.pid.unwrap() as i32),
        Some(WaitPidFlag::WNOHANG),
    ) {
        Ok(res) => match res {
            WaitStatus::Exited(_, _) | WaitStatus::Signaled(_, _, _) => {
                state.status = ContainerStatus::Stopped;
                state.save_to(container_path);
            }
            _ => (),
        },
        Err(err) => {
            if err.as_errno() != Some(Errno::ECHILD) {
                panic!("查询进程状态失败: {}", err);
            }
        }
    }
}
