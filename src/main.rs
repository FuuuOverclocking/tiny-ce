use tiny_ce::actions::{create, CreateOptions};
use tiny_ce::cpp_exports;
use chrono;

fn main() {
    test_cpp_interop();

    let datetime = chrono::offset::Local::now();
    let container_id = format!("container-centos-{}", datetime.format("%m%d-%H%M%S"));

    println!("容器 ID = {}", container_id);

    create(CreateOptions {
        id: container_id,
        bundle: "./test/centos".to_string(),
    });
}

fn test_cpp_interop() {
    unsafe {
        println!("test child_process");
        cpp_exports::setup_args(114, 514);
        let a = cpp_exports::read_a();
        let b = cpp_exports::read_b();
        println!("a = {}, b = {}", a, b);

        cpp_exports::cgroups_test();
        cpp_exports::netns_test();
    }
}