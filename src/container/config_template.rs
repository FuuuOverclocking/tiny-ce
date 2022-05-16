use super::types::Container::Config;
use super::types::Container::ConfigTypes::NamespaceType::*;
use super::types::Container::ConfigTypes::*;

impl Config {
    pub fn new_template() -> Box<Config> {
        Box::new(Config {
            root: Root {
                path: "rootfs".to_string(),
            },
            process: Process {
                cwd: "/".to_string(),
                env: Some(vec![
                    "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin".to_string(),
                ]),
                args: Some(vec!["sh".to_string()]),
                user: ProcessUser { uid: 0, gid: 0 },
            },
            hostname: Some("fu-asus".to_string()),
            namespaces: vec![
                Namespace {
                    r#type: Pid,
                    path: None,
                },
                Namespace {
                    r#type: Network,
                    path: None,
                },
                Namespace {
                    r#type: Mount,
                    path: None,
                },
                Namespace {
                    r#type: Ipc,
                    path: None,
                },
                Namespace {
                    r#type: Uts,
                    path: None,
                },
                Namespace {
                    r#type: User,
                    path: None,
                },
                Namespace {
                    r#type: Cgroup,
                    path: None,
                },
            ],
            uid_mappings: None,
            gid_mappings: None,
            cgroups_path: None,
            resources: Resources {
                memory: ResourcesMemory {
                    limit: 512 * 1024 * 1024,
                    reservation: 512 * 1024 * 1024,
                    swap: 512 * 1024 * 1024,
                },
                cpu: ResourcesCPU {
                    shares: 1024,
                    quota: 1000000,
                    period: 500000,
                },
            },
        })
    }
}
