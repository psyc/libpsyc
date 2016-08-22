use std::env;
use std::process::Command;
use std::path::*;

fn main() {   
    Command::new("make")
        .arg("-C")
        .arg("..")
        .output()
        .unwrap_or_else(|e| panic!("failed to execute process: {}", e));

    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let path = Path::new(&manifest_dir).parent().unwrap().join("lib");

    println!("cargo:rustc-link-search=native={}", path.display());
    println!("cargo:rerun-if-changed={}", path.display());
    println!("cargo:rustc-link-lib=static=psyc");
}
