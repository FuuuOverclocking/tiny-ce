#ifndef _CHILD_PROCESS_ARGS_HPP
#define _CHILD_PROCESS_ARGS_HPP

#include "vendors/json.hpp"
#include <string>

struct ChildProcessArgs {
    nlohmann::json config;
    const char *config_path;
    const char *init_lock_path;
    const char *sock_path;
    std::string resolved_rootfs;
    int pty_socket;
    int init_lock_sock;
    int container_sock;
    int container_receive_runtime_sock;
};

#endif