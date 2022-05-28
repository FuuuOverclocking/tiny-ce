use crate::container::{config::Namespace, ContainerConfig};
use nix::unistd::Pid;
use std::collections::HashMap;

fn extract_userns_from_config(config: &ContainerConfig) -> Option<&Namespace> {
    config
        .linux
        .as_ref()
        .unwrap()
        .namespaces
        .iter()
        .filter(|ns| ns.namespace == "user")
        .next()
}

pub fn should_setup_mapping(config: &ContainerConfig) -> bool {
    let user_namespace = extract_userns_from_config(config);
    user_namespace.is_some() && user_namespace.unwrap().path.is_none()
}

pub fn setup_mapping(config: &ContainerConfig, pid: &Pid) {
    let config_linux = config.linux.as_ref().unwrap();
    let uid_mappings = config_linux
        .uid_mappings
        .as_ref()
        .expect("使用 user namespace (未指定 path), 但 uid mapping 为空.");
    let gid_mappings = config_linux
        .gid_mappings
        .as_ref()
        .expect("使用 user namespace (未指定 path), 但 gid mapping 为空.");
    if uid_mappings.is_empty() {
        panic!("使用 user namespace (未指定 path), 但 uid mapping 为空.");
    }
    if gid_mappings.is_empty() {
        panic!("使用 user namespace (未指定 path), 但 gid mapping 为空.");
    }
    // TODO
}
