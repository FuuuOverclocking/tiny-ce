use serde::{Deserialize, Serialize};
use std::{io::Write, path::Path, fs};

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
}

impl ContainerState {
    pub fn new(id: &String, bundle: &String) -> Self {
        ContainerState {
            id: id.clone(),
            status: ContainerStatus::Creating,
            pid: None,
            bundle: Path::new(bundle)
                .canonicalize()
                .unwrap()
                .to_string_lossy()
                .to_string(),
        }
    }

    pub fn save_to(&self, container_path: &Path) {
        std::fs::create_dir_all(container_path).unwrap();

        let state_json_path = container_path.join("state.json");

        if state_json_path.exists() {
            fs::remove_file(&state_json_path).unwrap();
        }
        let mut state_file = std::fs::OpenOptions::new()
            .write(true)
            .create(true)
            .open(&state_json_path)
            .expect("无法打开 state.json");
        let json = serde_json::to_string(self).unwrap();
        let json_bytes = json.as_bytes();
        state_file
            .write_all(json_bytes)
            .expect("写入 state.json 失败");
    }
}

impl TryFrom<&Path> for ContainerState {
    type Error = String;

    fn try_from(path: &Path) -> core::result::Result<Self, Self::Error> {
        let state_json = std::fs::read_to_string(path.join("state.json"))
            .map_err(|err| format!("未在 {:?} 找到 state.json {}", path, err))?;

        let state: ContainerState =
            serde_json::from_str(&state_json).map_err(|_| "序列化 state.json 失败".to_string())?;
        Ok(state)
    }
}
