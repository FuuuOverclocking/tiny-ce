use crate::container::{ContainerConfig, ContainerState};

use std::path::Path;

const CONTAINER_ROOT_PATH: &'static str = "/tmp/tiny-ce";

pub struct CreateOptions {
    pub id: String,
    pub bundle: String,
}

pub fn create(options: CreateOptions) {
    let bundle = Path::new(&options.bundle);
    let config_path = bundle.join("config.json");
    let config = ContainerConfig::read_to_config(&config_path);

    let container_path_str = format!("{}/{}", CONTAINER_ROOT_PATH, options.id);
    let container_path = Path::new(&container_path_str);

    let state = ContainerState::new(&options.id, None, &options.bundle);
    state.save_to(container_path);
}
