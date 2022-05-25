# Tiny CE 的规范

## 支持的操作

- `create(CreateOptions { id, bundle })`
- `start(StartOptions { id })`
- `delete(DeleteOptions { id })`

用例:

```rust
use chrono;
use tiny_ce::actions::{
    create,
    CreateOptions,
    start,
    StartOptions
};

fn main() {
    let datetime = chrono::offset::Local::now();
    let container_id = format!("container-centos-{}", datetime.format("%m%d-%H%M%S"));

    // 创建容器
    create(CreateOptions {
        id: container_id.clone(),
        bundle: "./test/centos".to_string(),
    });

    // 启动容器
    start(StartOptions {
        id: container_id.clone(),
    });
}
```

## 支持的 config.json 字段

```json
{
    "root": {
        "path": "rootfs"
    },
    "process": {
        "cwd": "/",
        "env": [
            "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
        ],
        "args": ["bash"],
        "user": {
            "uid": 0,
            "gid": 0
        }
    },
    "hostname": "fu-asus",
    "linux": {
        "namespaces": [
            { "type": "pid", "path": "(可选) 指向现有命名空间的路径." },
            { "type": "network" },
            { "type": "mount" },
            { "type": "ipc" },
            { "type": "uts" },
            { "type": "user" },
            { "type": "cgroup" }
        ],
        "cgroupsPath": "(实验性) 待定",
        "resources": {
            "memory": {
                "limit": 536870912,
                "reservation": 536870912,
                "swap": 536870912
            },
            "cpu": {
                "shares": 1024,
                "quota": 1000000,
                "period": 500000
            }
        }
    }
}
```