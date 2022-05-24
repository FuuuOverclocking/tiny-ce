extern crate bindgen;
extern crate cc;
extern crate glob;

use glob::glob;
use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rerun-if-changed=src/cpp");

    let bindings = bindgen::Builder::default()
        .header("src/cpp/exports.hpp")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("cpp_exports.rs"))
        .expect("Couldn't write bindings!");

    build_cpp_child_process();
    build_libcgroups();
    build_libnetns();
}

fn build_cpp_child_process() {
    let files = glob("src/cpp/child_process/**/*.cpp")
        .unwrap()
        .map(|r| r.unwrap());

    cc::Build::new()
        .cpp(true)
        .flag("-std=c++17")
        .files(files)
        .compile("child_process.a");
}

fn build_libcgroups() {
    let files = glob("src/cpp/libcgroups/**/*.cpp")
        .unwrap()
        .map(|r| r.unwrap());

    cc::Build::new()
        .cpp(true)
        .flag("-std=c++17")
        .files(files)
        .compile("libcgroups.a");
}

fn build_libnetns() {
    let files = glob("src/cpp/libnetns/**/*.cpp")
        .unwrap()
        .map(|r| r.unwrap());

    cc::Build::new()
        .cpp(true)
        .flag("-std=c++17")
        .files(files)
        .compile("libnetns.a");
}
