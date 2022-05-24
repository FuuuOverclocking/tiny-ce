#ifndef _CHILD_PROCESS_ENTRY_HPP
#define _CHILD_PROCESS_ENTRY_HPP

#include "exports.hpp"

struct ChildProcessArgs {
    const char *config_path;
    const char *init_lock_path;
    const char *sock_path;
    int pty_socket;
};

void _setup_args(const char *config_path, const char *init_lock_path,
                 const char *sock_path, int pty_socket);
int _child_main();

#endif