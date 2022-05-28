mod actions;
mod container;
pub mod cpp_exports; // for cpp test
pub use actions::*;

#[cfg(test)]
mod tests {
    use super::cpp_exports;
    use super::{create, delete, start, CONTAINER_ROOT_PATH};
    use chrono;
    use std::fs;
    use std::path::Path;
    use std::thread;

    /// 创建一个容器并启动.
    #[test]
    fn create_start() {
        check_test_rootfs_existence();

        let datetime = chrono::offset::Local::now();
        let container_id = format!("container-centos-{}", datetime.format("%m%d-%H%M%S"));
        let bundle = "./test/centos".to_string();
        let container_path = format!("{}/{}", CONTAINER_ROOT_PATH, container_id);

        println!("启动和创建容器:");
        println!("    bundle = ./test/centos");
        println!("    id = {}", container_id);
        println!("    容器临时文件路径 = {}", container_path);
        println!();

        // 通过 cargo test 启动进程时, 栈的容量小于正常运行, 为 2MB.
        // 启动子进程, 设置栈的大小为 8MB.
        let child = thread::Builder::new()
            .stack_size(8 * 1024 * 1024)
            .spawn(move || {
                println!("创建中...");
                create(&container_id, &bundle);

                println!("启动中...");
                start(&container_id);

                println!("已启动.");
            })
            .unwrap();

        child.join().unwrap();
    }

    fn check_test_rootfs_existence() {
        let path = Path::new("./test/centos");
        if let Ok(mut path) = fs::read_dir(path) {
            if path.next().is_some() {
                return;
            }
        }

        println!("未在 ./test/centos/rootfs 下找到 CentOS 的根文件目录.");
        println!("执行以下命令以准备 centos 容器的 rootfs:");
        println!("> cd ./test/centos && mkdir rootfs");
        println!("> docker export $(docker create centos:centos7.9.2009) | tar -C rootfs -xvf -");
        panic!();
    }

    /// 删除所有容器, 并打印被删除容器的 id.
    #[test]
    fn delete_all() {
        let container_path_list = fs::read_dir(CONTAINER_ROOT_PATH)
            .expect(format!("无法访问 {} .", CONTAINER_ROOT_PATH).as_str());

        let container_id_list: Vec<String> = container_path_list
            .into_iter()
            .map(|path| path.unwrap().path())
            .filter(|path| path.is_dir())
            .map(|path| path.file_name().unwrap().to_string_lossy().to_string())
            .collect();

        for id in container_id_list.iter() {
            delete(id);
            println!("id = {} 的容器被删除了.", id);
        }

        println!("已删除 {} 个容器.", container_id_list.len());
    }

    #[test]
    fn cpp_interop() {
        unsafe {
            println!("测试 C++ cgroups");
            cpp_exports::cgroups_test();

            println!("测试 C++ netns");
            cpp_exports::netns_test();
        }
    }
}
