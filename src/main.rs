use chrono;
use std::ffi::CString;
use tiny_ce::actions::{create, CreateOptions};
use tiny_ce::cpp_exports;

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
        println!("测试 C++ 子进程");
        let config_path = CString::new("./test/centos/config.json").expect("CString::new failed");
        let init_lock_path = CString::new("").expect("CString::new failed");
        let sock_path = CString::new("").expect("CString::new failed");
        cpp_exports::setup_args(
            config_path.as_ptr(),
            init_lock_path.as_ptr(),
            sock_path.as_ptr(),
            -1,
        );
        let exit_code = cpp_exports::child_main();
        println!("child_main(): exit_code={}", exit_code);

        cpp_exports::cgroups_test();
        cpp_exports::netns_test();
    }
}
