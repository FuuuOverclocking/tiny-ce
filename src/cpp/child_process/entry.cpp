#include "entry.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <string>

using nlohmann::json;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;

ChildProcessArgs *args = nullptr;

void _setup_args(const char *config_path, const char *init_lock_path,
                 const char *sock_path, int pty_socket) {
    args = new ChildProcessArgs;
    args->config_path = config_path;
    args->init_lock_path = init_lock_path;
    args->sock_path = sock_path;
    args->pty_socket = pty_socket;
}

int _child_main() {
    assert(args != nullptr);

    json config;
    ifstream(args->config_path) >> config;

    auto rootfs = config["root"]["path"].get<string>();

    cout << "容器的 rootfs 是: " << rootfs << endl;
    cout << "容器的 process.args 是: "
         << config["process"]["args"][0].get<string>() << endl;
    return 0;
}