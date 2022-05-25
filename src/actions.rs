use nix::errno::Errno;
use nix::sys::wait::{waitpid, WaitPidFlag, WaitStatus};
use nix::unistd::Pid;

use crate::container::{
    fork_container, ContainerConfig, ContainerState, ContainerStatus, IpcChannel, IpcParent,
};
use std::fs;
use std::path::{Path, PathBuf};

const CONTAINER_ROOT_PATH: &'static str = "/tmp/tiny-ce";

pub fn create(id: &String, bundle: &String) {
    let bundle_path = fs::canonicalize(Path::new(bundle)).unwrap();
    let config_path = bundle_path.join("config.json");
    let config = ContainerConfig::read_to_config(&config_path);

    let container_path_str = format!("{}/{}", CONTAINER_ROOT_PATH, id);
    let container_path = Path::new(&container_path_str);

    let mut state = ContainerState::new(id, None, bundle);
    println!("容器状态: {:?}", state);
    state.save_to(container_path);

    let init_lock_path = format!("{}/init.sock", container_path_str);
    let init_lock = IpcParent::new(&init_lock_path);
    let sock_path = format!("{}/container.sock", container_path.display());

    let pid = fork_container(&config, &config_path, &init_lock_path, &sock_path);

    let response = init_lock.wait();
    if !response.eq("ok") {
        panic!("子进程未发送 ok, 发送了 {}", response);
    }
    init_lock.close();

    let ipc_channel = IpcChannel::connect(&sock_path);

    loop {
        let msg = ipc_channel.recv();
        if msg.starts_with("error") {
            panic!("子进程发送了 {}", msg);
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
    let ipc_channel = IpcChannel::connect(&sock_path);
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
        panic!("试图 delete 尚未创建或运行的容器.")
    }

    check_stopped(&mut state, &container_path);

    if state.status != ContainerStatus::Stopped {
        panic!("试图 delete 仍在运行的容器.")
    }
    println!("容器状态: {:?}", state);
    if std::fs::remove_dir_all(Path::new(CONTAINER_ROOT_PATH).join(id)).is_err() {
        println!("删除容器失败.");
    }
}

fn check_stopped(state: &mut ContainerState, container_path: &PathBuf) {
    match waitpid(Pid::from_raw(state.pid.unwrap() as i32), Some(WaitPidFlag::WNOHANG)) {
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