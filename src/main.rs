use chrono;
use std::{env, fs};
use tiny_ce::{create, delete, start, CONTAINER_ROOT_PATH};

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() == 3 && args[1] == "cs" {
        create_start_centos(&args[2]);
        return;
    }

    if args.len() == 2 && args[1] == "del" {
        delete_all();
        return;
    }

    let help = [
        "Usage: tiny-ce [command]",
        "",
        "Commands:",
        "  cs <bundle-path>     创建并启动一个容器",
        "  del                  删除所有容器",
        "",
        "Example: 在 test/centos 下制作容器的 rootfs 并启动",
        "  $ cd ./test/centos && mkdir rootfs",
        "  $ docker export $(docker create centos:centos7.9.2009) | tar -C rootfs -xvf -",
        "  $ <somewhere>/tiny-ce cs ./",
    ]
    .join("\n");
    println!("{}", help);
}

/// 创建并启动一个容器
fn create_start_centos(bundle: &String) {
    let datetime = chrono::offset::Local::now();
    let container_id = format!("container-centos-{}", datetime.format("%m%d-%H%M%S"));
    let container_path = format!("{}/{}", CONTAINER_ROOT_PATH, container_id);

    println!("启动和创建容器:");
    println!("    bundle = {}", bundle);
    println!("    id = {}", container_id);
    println!("    容器临时文件路径 = {}", container_path);
    println!();

    println!("创建中...");
    create(&container_id, &bundle);

    println!("启动中...");
    start(&container_id);
}

/// 删除所有容器, 并打印被删除容器的 id.
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
