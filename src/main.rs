use actions::CreateOptions;

mod container;

fn main() {
    actions::create(CreateOptions {
        id: "centos-container-id".to_string(),
        bundle: "./exp/centos".to_string(),
    })
}

mod actions {
    use std::path::Path;
    use crate::container::{ContainerConfig, ContainerState};

    const CONTAINER_ROOT_PATH: &'static str = "/tmp/tiny-ce";

    pub struct CreateOptions {
        pub id: String,
        pub bundle: String,
    }
    pub fn create(opts: CreateOptions) {
        let bundle = Path::new(&opts.bundle);
        let config_path = bundle.join("config.json");

        let config = ContainerConfig::read_json_to_config(&config_path);

        let container_path_str = format!("{}/{}", CONTAINER_ROOT_PATH, opts.id);
        let container_path = Path::new(&container_path_str);

        let state = ContainerState::new(&opts.id, None, &opts.bundle);
        state.save_to(container_path);
        
    }
}
