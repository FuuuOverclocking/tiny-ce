use tiny_ce::*;

fn main() {
    let mut config = Container::Config::new_template();
    config.root.path = "/home/fu/work/22-2/tiny-ce/exp/centos/rootfs".to_string();
    

    println!("{}", "done");
}
