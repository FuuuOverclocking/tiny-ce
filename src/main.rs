use chrono;
use tiny_ce::actions::{create, CreateOptions};
use tiny_ce::cpp_exports;

fn main() {
    test_cpp_interop();

    let datetime = chrono::offset::Local::now();
    let container_id = format!("container-centos-{}", datetime.format("%m%d-%H%M%S"));

    create(CreateOptions {
        id: container_id,
        bundle: "./test/centos".to_string(),
    });
}

fn test_cpp_interop() {
    unsafe {
        println!("测试 C++ cgroups");
        cpp_exports::cgroups_test();

        println!("测试 C++ netns");
        cpp_exports::netns_test();
    }
}
