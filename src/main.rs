#![allow(unused)]

use chrono;
use std::{thread, time::Duration};
use tiny_ce::actions::{create, delete, start, CreateOptions, DeleteOptions, StartOptions};
use tiny_ce::cpp_exports;

fn main() {
    // delete(DeleteOptions { id: "container-centos-0525-161117".to_string() });
    // return;

    test_cpp_interop();

    let datetime = chrono::offset::Local::now();
    let container_id = format!("container-centos-{}", datetime.format("%m%d-%H%M%S"));

    create(CreateOptions {
        id: container_id.clone(),
        bundle: "./test/centos".to_string(),
    });

    start(StartOptions {
        id: container_id.clone(),
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
