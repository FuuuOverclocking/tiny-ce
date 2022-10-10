use crate::cpp_exports;
use nix::{
    sched::{clone, CloneFlags},
    unistd::Pid,
};
use oci_spec::runtime::{LinuxNamespaceType, Spec};
use std::{ffi::CString, os::unix::prelude::OsStrExt, path::Path};

pub fn fork_container(
    config: &Spec,
    config_path: &Path,
    init_lock_path: &Path,
    sock_path: &Path,
) -> Pid {
    let config_path = CString::new(config_path.as_os_str().as_bytes()).unwrap();
    let init_lock_path = CString::new(init_lock_path.as_os_str().as_bytes()).unwrap();
    let sock_path = CString::new(sock_path.as_os_str().as_bytes()).unwrap();

    unsafe {
        cpp_exports::setup_args(
            config_path.as_ptr(),
            init_lock_path.as_ptr(),
            sock_path.as_ptr(),
            -1,
        );
    }

    const STACK_SIZE: usize = 4 * 1024 * 1024; // 4 MB
    let stack = Box::leak(Box::new([0; STACK_SIZE]));

    let namespaces = config
        .linux()
        .as_ref()
        .unwrap()
        .namespaces()
        .as_ref()
        .unwrap();
    let clone_flags = namespaces
        .into_iter()
        .map(|ns| match ns.typ() {
            LinuxNamespaceType::Pid => CloneFlags::CLONE_NEWPID,
            LinuxNamespaceType::Network => CloneFlags::CLONE_NEWNET,
            LinuxNamespaceType::Mount => CloneFlags::CLONE_NEWNS,
            LinuxNamespaceType::Ipc => CloneFlags::CLONE_NEWIPC,
            LinuxNamespaceType::Uts => CloneFlags::CLONE_NEWUTS,
            LinuxNamespaceType::User => CloneFlags::CLONE_NEWUSER,
            LinuxNamespaceType::Cgroup => CloneFlags::CLONE_NEWCGROUP,
        })
        .reduce(|a, b| a | b);

    let clone_flags = match clone_flags {
        Some(flags) => flags,
        None => CloneFlags::empty(),
    };

    let child_fn = || unsafe { cpp_exports::child_main() as isize };
    let child = clone(Box::new(child_fn), stack, clone_flags, None).expect("clone 失败");

    return child;
}
