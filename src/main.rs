#![allow(unused)]

use chrono;
use std::{thread, time::Duration};
use tiny_ce::{create, delete, start};
use tiny_ce::cpp_exports;

fn main() {
    test_cpp_interop();

    let datetime = chrono::offset::Local::now();
    let container_id = format!("container-centos-{}", datetime.format("%m%d-%H%M%S"));
    let bundle = "./test/centos".to_string();

    create(&container_id, &bundle);
    start(&container_id);
    // delete(&"container-centos-0525-161117".to_string() });
}

fn test_cpp_interop() {
    unsafe {
        println!("测试 C++ cgroups");
        cpp_exports::cgroups_test();

        println!("测试 C++ netns");
        cpp_exports::netns_test();
    }
}
