use tiny_ce_proto::*;

fn main() {
    let mut config = Container::Config::new_template();
    config.root.path = "/home/fu/work/tiny-ce-proto/centos-rootfs".to_string();
    

    println!("{}", "done");
}
