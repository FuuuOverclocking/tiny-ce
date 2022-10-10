use serde::{Deserialize, Serialize};
use std::{ffi::OsString, fmt, fmt::Display, fs, io::Write, path::Path};

#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub enum ContainerStatus {
    Creating,
    Created,
    Running,
    Stopped,
}

#[derive(Clone, PartialEq, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ContainerState {
    pub id: String,
    pub status: ContainerStatus,
    pub pid: Option<usize>,
    pub bundle: OsString,
}

impl ContainerState {
    pub fn new(id: &str, bundle: &Path) -> Self {
        ContainerState {
            id: id.to_string(),
            status: ContainerStatus::Creating,
            pid: None,
            bundle: bundle.canonicalize().unwrap().as_os_str().to_os_string(),
        }
    }

    pub fn save_to(&self, container_path: &Path) {
        std::fs::create_dir_all(container_path).unwrap();

        let state_json_path = container_path.join("state.json");

        if state_json_path.exists() {
            fs::remove_file(&state_json_path).unwrap();
        }

        let json = serde_json::to_string(self).unwrap();
        let mut state_file = std::fs::OpenOptions::new()
            .write(true)
            .create(true)
            .open(&state_json_path)
            .expect("无法打开 state.json");
        state_file
            .write_all(json.as_bytes())
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

impl Display for ContainerState {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "容器状态: id = {}, status = {:?}, pid = {:?}, bundle = {}",
            self.id, self.status, self.pid, self.bundle.to_string_lossy()
        )
    }
}
