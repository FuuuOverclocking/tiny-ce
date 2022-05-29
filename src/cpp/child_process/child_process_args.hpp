#ifndef _CHILD_PROCESS_ARGS_HPP
#define _CHILD_PROCESS_ARGS_HPP

#include "vendors/json.hpp"

using nlohmann::json;

struct ChildProcessArgs {
    json config;
    const char *config_path;
    const char *init_lock_path;
    const char *sock_path;
    int pty_socket;
    int init_lock_sock;
    int container_sock;
};

#endif