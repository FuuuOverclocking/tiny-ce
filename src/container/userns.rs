use crate::container::{config::Namespace, ContainerConfig};
use nix::unistd::Pid;
use std::{path::{PathBuf, Path}, fs};

use super::config::UgidMapping;

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
    if uid_mappings.len() != 1 {
        panic!("使用 user namespace (未指定 path), 但 uid mapping 数量不是 1.");
    }
    if gid_mappings.len() != 1 {
        panic!("使用 user namespace (未指定 path), 但 gid mapping 数量不是 1.");
    }

    let uid_path = PathBuf::from(format!("/proc/{pid}/uid_map"));
    let gid_path = PathBuf::from(format!("/proc/{pid}/gid_map"));

    write_id_mapping(&uid_path.as_path(), uid_mappings);
    write_id_mapping(&gid_path.as_path(), gid_mappings);
}

fn write_id_mapping(map_path: &Path, mappings: &Vec<UgidMapping>) {
    let mapping_str = mappings.first().map(|m| {
        format!("{} {} {}", m.container_id, m.host_id, m.size)
    }).unwrap();
    fs::write(map_path, mapping_str).expect("写入 uid 或 gid mapping 失败");
}
