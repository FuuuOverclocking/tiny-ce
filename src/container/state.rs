use serde::{Deserialize, Serialize};
use std::{collections::HashMap, io::Write, path::Path};

#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub enum ContainerStatus {
    Creating,
    Created,
    Running,
    Stopped,
}

#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ContainerState {
    pub id: String,
    pub status: ContainerStatus,
    pub pid: Option<usize>,
    pub bundle: String,
    pub annotations: Option<HashMap<String, String>>,
}

impl ContainerState {
    pub fn new(id: &String, pid: Option<usize>, bundle: &String) -> Self {
        ContainerState {
            id: id.clone(),
            status: ContainerStatus::Creating,
            pid,
            bundle: Path::new(bundle)
                .canonicalize()
                .unwrap()
                .to_string_lossy()
                .to_string(),
            annotations: Some(HashMap::<String, String>::new()),
        }
    }

    pub fn save_to(&self, root_path: &Path) {
        std::fs::create_dir_all(root_path).unwrap();

        let mut state_file = std::fs::OpenOptions::new()
            .write(true)
            .create(true)
            .open(root_path.join("state.json"))
            .expect("无法打开 state.json");
        state_file
            .write_all(serde_json::to_string(self).unwrap().as_bytes())
            .expect("写入 state.json 失败");
    }
}