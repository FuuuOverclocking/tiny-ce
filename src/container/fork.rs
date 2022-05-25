use crate::container::{config::Namespace, ContainerConfig};
use crate::cpp_exports;
use nix::{
    sched::{clone, CloneFlags},
    unistd::Pid,
};
use std::{ffi::CString, path::PathBuf};

pub fn fork_container(
    config: &ContainerConfig,
    config_path: &PathBuf,
    init_lock_path: &String,
    sock_path: &String,
    // pty_socket: &Option<PtySocket>,
) -> Pid {
    let config_path = CString::new(config_path.to_str().unwrap()).expect("CString::new failed");
    let init_lock_path = CString::new(init_lock_path.as_str()).expect("CString::new failed");
    let sock_path = CString::new(sock_path.as_str()).expect("CString::new failed");
    unsafe {
        cpp_exports::setup_args(
            config_path.as_ptr(),
            init_lock_path.as_ptr(),
            sock_path.as_ptr(),
            -1,
        );
    }

    const STACK_SIZE: usize = 4 * 1024 * 1024; // 4 MB
    let ref mut stack: [u8; STACK_SIZE] = [0; STACK_SIZE];

    let namespaces = config.linux.as_ref().unwrap().namespaces.clone();
    let spec_namespaces = namespaces
        .into_iter()
        .map(|ns| to_flags(&ns))
        .reduce(|a, b| a | b);

    let clone_flags = match spec_namespaces {
        Some(flags) => flags,
        None => CloneFlags::empty(),
    };

    let child_fn = || unsafe { cpp_exports::child_main() as isize };
    let child = clone(Box::new(child_fn), stack, clone_flags, None);

    return child.expect("clone 失败");
}

fn to_flags(namespace: &Namespace) -> CloneFlags {
    match namespace.namespace.as_str() {
        "pid" => CloneFlags::CLONE_NEWPID,
        "network" | "net" => CloneFlags::CLONE_NEWNET,
        "mount" | "mnt" => CloneFlags::CLONE_NEWNS,
        "ipc" => CloneFlags::CLONE_NEWIPC,
        "uts" => CloneFlags::CLONE_NEWUTS,
        "user" => CloneFlags::CLONE_NEWUSER,
        "cgroup" => CloneFlags::CLONE_NEWCGROUP,
        _ => panic!("unknown namespace {}", namespace.namespace),
    }
}
