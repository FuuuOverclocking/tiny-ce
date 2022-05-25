# Tiny CE

基于 Linux 的 Namespace 和 Cgroup 实现的一个简单的容器引擎.

## Quick start

事先确认:

1. 使用常见 Linux 发行版的较新版本, 或 Windows 10/11 WSL
2. 已安装 Clang 5.0 及以上版本, 指南: https://rust-lang.github.io/rust-bindgen/requirements.html
3. 已安装 Rust, 指南: https://www.rust-lang.org/tools/install

```bash
git clone https://github.com/FuuuOverclocking/tiny-ce.git
cd tiny-ce

./run.sh  # tiny-ce 需要 root 权限以运行, 将调用 sudo
```

已确认在以下环境中可用:

- Windows 11 x64, WSL2, Ubuntu 20.04.1
- rustc 1.60.0
- cargo 1.60.0
- gcc 9.4.0
- clang 10.0.0-4ubuntu1
- 测试镜像: DockerHub/centos, tag=centos7.9.2009

## Tiny CE 的目标

- 实现进程, 用户, 文件系统, 网络等方面的隔离
- 能够在 Ubuntu 系统上运行 CentOS 环境
- 能够实现同一操作系统下两个容器之间的网络通信
- 能够为容器分配定量的 CPU 和内存资源

## Tiny CE 的规范

Tiny CE 的规范是 OCI 规范的一个较小的子集, 参见 [docs/tiny-ce-spec.md](docs/oci-runtime-spec-1.1-alpha/index.md).

### OCI Runtime 标准翻译

翻译自: https://github.com/opencontainers/runtime-spec/commit/27924127bf391ea7691924c6dcb01f3369d69fe2

只翻译了感兴趣的部分, 好累..

参见 [docs/oci-runtime-spec-1.1-alpha/index.md](docs/oci-runtime-spec-1.1-alpha/index.md).

## 关于 create, start 的过程：

### 方法一：

借助 `/bin/unshare` 和  `/bin/cgexec`.

- 容器引擎进程启动 `cgexec`
    - `cgexec` 启动 `unshare`
        - `unshare` 启动 `config.process.args[0]` 子进程.

### 方法二：

使用 C 接口的系统调用。

1. 利用 `youki/libcgroups` 完成 cgroup 新建/删除 (实际上利用文件系统 mount 和读写)
2. (不是 OCI 标准的内容) 利用 ip 命令在 `/var/run/netns/` 下创建 net namespace
3. 用 clone 系统调用, 创建子进程, 在创建时指定子进程的命名空间
4. 子进程用 setns 系统调用进入 net 和 cgroup 命名空间, 以下都是子进程要干的活
5. 挂载 rootfs:
   
    参考
    - https://www.cnblogs.com/sparkdev/p/9045563.html
    - https://man7.org/linux/man-pages/man2/pivot_root.2.html 中的 EXAMPLES

    1. `mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL)`
    2. `mount(new_root, new_root, NULL, MS_BIND, NULL)`
6. 用 mknod 系统调用, 在 `rootfs` 下创建 OCI 标准要求的 Linux 默认设备:
   
    ```rust
    vec![
        Device {
            path: String::from("/dev/null"),
            device_type: String::from("c"),
            major: 1,
            minor: 3,
            file_mode: Some(0o066),
            uid: Some(0),
            gid: Some(0),
        },
        Device {
            path: String::from("/dev/zero"),
            device_type: String::from("c"),
            major: 1,
            minor: 5,
            file_mode: Some(0o066),
            uid: Some(0),
            gid: Some(0),
        },
        Device {
            path: String::from("/dev/full"),
            device_type: String::from("c"),
            major: 1,
            minor: 7,
            file_mode: Some(0o066),
            uid: Some(0),
            gid: Some(0),
        },
        Device {
            path: String::from("/dev/random"),
            device_type: String::from("c"),
            major: 1,
            minor: 8,
            file_mode: Some(0o066),
            uid: Some(0),
            gid: Some(0),
        },
        Device {
            path: String::from("/dev/urandom"),
            device_type: String::from("c"),
            major: 1,
            minor: 9,
            file_mode: Some(0o066),
            uid: Some(0),
            gid: Some(0),
        },
        Device {
            path: String::from("/dev/tty"),
            device_type: String::from("c"),
            major: 5,
            minor: 0,
            file_mode: Some(0o066),
            uid: Some(0),
            gid: Some(0),
        },
        Device {
            path: String::from("/dev/ptmx"),
            device_type: String::from("c"),
            major: 5,
            minor: 2,
            file_mode: Some(0o066),
            uid: Some(0),
            gid: Some(0),
        },
    ]
    ```
7. 根据 OCI 标准创建符号链接:

    ```
    /proc/self/fd   -> rootfs/dev/fd
    /proc/self/fd/0 -> rootfs/dev/stdin
    /proc/self/fd/1 -> rootfs/dev/stdout
    /proc/self/fd/2 -> rootfs/dev/stderr
    ```
8. cd rootfs
9. pivot_root 系统调用
10. unmount 旧根目录
11. sethostname 系统调用
12. 修改环境变量
13. setuid, setgid 系统调用
14. 进入 `process.cwd`, 执行 `process.args`

## 参考资料

- OCI 标准
    - https://github.com/opencontainers/runtime-spec
- CNI 标准
    - https://github.com/containernetworking/cni/blob/main/SPEC.md
- Container Runtime in Rust
    - https://github.com/penumbra23/pura
    - https://itnext.io/container-runtime-in-rust-part-0-7af709415cda
    - https://itnext.io/container-runtime-in-rust-part-i-7bd9a434c50a
    - https://itnext.io/container-runtime-in-rust-part-ii-9c88e99d8cbc
- youki
    - 生产级的
    - https://github.com/containers/youki
    - https://containers.github.io/youki/
- docker run
    - https://docs.docker.com/engine/reference/run/