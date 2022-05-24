use serde::{Deserialize, Serialize};
use std::{fs, path::Path};

#[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ContainerConfig {
    pub root: Root,
    pub process: Process,
    pub hostname: Option<String>,
    // pub hooks: Option<Hooks>,
    pub linux: Option<Linux>,
    // pub annotations: Option<HashMap<String, String>>,
}

impl ContainerConfig {
    pub fn read_to_config(path: &Path) -> Self {
        let json_text = fs::read_to_string(path).expect("读取 config.json 失败");
        serde_json::from_str(&json_text).expect("config.json 不是有效的配置文件")
    }
}

#[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Root {
    pub path: String,
}

#[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Process {
    // pub terminal: Option<bool>,
    pub cwd: String,
    pub env: Option<Vec<String>>,
    pub args: Option<Vec<String>>,
    pub user: User,
}

#[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct User {
    pub uid: usize,
    pub gid: usize,
}

#[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Linux {
    pub namespaces: Vec<Namespace>,
    // pub uid_mappings: Option<Vec<UgidMapping>>,
    // pub gid_mappings: Option<Vec<UgidMapping>>,
    pub cgroups_path: Option<String>,
    pub resources: Resources,
}

#[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Namespace {
    #[serde(rename = "type")]
    pub namespace: String,
    pub path: Option<String>,
}

// #[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
// #[serde(rename_all = "camelCase")]
// pub struct UgidMapping {
//     #[serde(rename = "containerID")]
//     pub container_id: i64,
//     #[serde(rename = "hostID")]
//     pub host_id: i64,
//     pub size: i64,
// }

#[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Resources {
    pub memory: Memory,
    pub cpu: Cpu,
}

#[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Memory {
    pub limit: Option<i64>,
    pub reservation: Option<i64>,
    pub swap: Option<i64>,
}

#[derive(Default, Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Cpu {
    pub shares: Option<u64>,
    pub quota: Option<i64>,
    pub period: Option<u64>,
}
