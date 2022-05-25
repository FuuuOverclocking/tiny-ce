#ifndef _CHILD_PROCESS_ENTRY_HPP
#define _CHILD_PROCESS_ENTRY_HPP

#include "exports.hpp"
#include "vendors/json.hpp"
#include <string>

using nlohmann::json;
using std::string;

struct ChildProcessArgs {
    json config;
    const char *config_path;
    const char *init_lock_path;
    const char *sock_path;
    int pty_socket;
};

void _setup_args(const char *config_path, const char *init_lock_path,
                 const char *sock_path, int pty_socket);
int _child_main();
char **args_to_argv(json &args);
string resolve_rootfs(const char *config_path, string rootfs);

extern "C" {
void setup_args(const char *config_path, const char *init_lock_path,
                const char *sock_path, int pty_socket) {
    _setup_args(config_path, init_lock_path, sock_path, pty_socket);
}
int child_main() { return _child_main(); }
}

#endif