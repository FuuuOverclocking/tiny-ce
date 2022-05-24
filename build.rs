extern crate bindgen;
extern crate cc;
extern crate glob;

use glob::glob;
use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rerun-if-changed=src/cpp/exports.hpp");

    let bindings = bindgen::Builder::default()
        .header("src/cpp/exports.hpp")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("cpp_exports.rs"))
        .expect("Couldn't write bindings!");

    cc::Build::new()
        .cpp(true)
        .files(
            glob("src/cpp/child_process/**/*.cpp")
                .unwrap()
                .map(|r| r.unwrap()),
        )
        .compile("child_process.a");

    cc::Build::new()
        .cpp(true)
        .files(
            glob("src/cpp/libcgroups/**/*.cpp")
                .unwrap()
                .map(|r| r.unwrap()),
        )
        .compile("libcgroups.a");

    cc::Build::new()
        .cpp(true)
        .files(
            glob("src/cpp/libnetns/**/*.cpp")
                .unwrap()
                .map(|r| r.unwrap()),
        )
        .compile("libnetns.a");
}
