# Tiny CE (prototype)

## 目的

用 Linux 操作系统的 Namespace 和 Cgroup 机制, 实现一个简单的容器引擎.

## 需求

- 实现进程, 用户, 文件系统, 网络等方面的隔离
- 能够在 Ubuntu 系统上运行 CentOS 环境
- 能够实现同一操作系统下两个容器之间的网络通信
- 能够为容器分配定量的 CPU 和内存资源

## OCI Runtime 标准翻译

翻译自: https://github.com/opencontainers/runtime-spec/commit/27924127bf391ea7691924c6dcb01f3369d69fe2

只翻译了感兴趣的部分, 好累..

参见 [docs/oci-runtime-spec-1.1-alpha/index.md](docs/oci-runtime-spec-1.1-alpha/index.md).

## Tiny CE 的规范

参见 [docs/tiny-ce-spec.md](docs/oci-runtime-spec-1.1-alpha/index.md).

## 参考资料

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