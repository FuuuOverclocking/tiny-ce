use crate::container::{
    fork_container, ContainerConfig, ContainerState, ContainerStatus, IpcChannel, IpcParent,
};
use std::fs;
use std::path::Path;

const CONTAINER_ROOT_PATH: &'static str = "/tmp/tiny-ce";

pub struct CreateOptions {
    pub id: String,
    pub bundle: String,
}

pub fn create(options: CreateOptions) {
    let bundle = fs::canonicalize(Path::new(&options.bundle)).unwrap();
    let config_path = bundle.join("config.json");
    let config = ContainerConfig::read_to_config(&config_path);

    let container_path_str = format!("{}/{}", CONTAINER_ROOT_PATH, options.id);
    let container_path = Path::new(&container_path_str);

    let mut state = ContainerState::new(&options.id, None, &options.bundle);
    println!("state: {:?}", state);
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
    state.save_to(container_path);
}
