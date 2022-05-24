#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "entry.hpp"
#include "json.hpp"

using nlohmann::json;
using std::cout, std::endl;
using std::ifstream;
using std::string;
using std::vector;

ChildProcessArgs *args = nullptr;

void _setup_args(const char *config_path, const char *init_lock_path,
                 const char *sock_path, int pty_socket) {
    args = new ChildProcessArgs;

    ifstream(config_path) >> args->config;
    args->init_lock_path = init_lock_path;
    args->sock_path = sock_path;
    args->pty_socket = pty_socket;
}

int _child_main() {
    assert(args != nullptr);

    auto rootfs = args->config["root"]["path"].get<string>();
    auto process_cwd = args->config["process"]["cwd"].get<string>();

    auto process_env = args->config["process"]["env"];
    assert(process_env.is_array());

    auto process_args = args->config["process"]["args"];
    assert(process_args.is_array());

    auto process_command = process_args[0].get<string>();
    auto process_argv = args_to_argv(process_args);
    auto process_argv_len = process_args.size();

    cout << "rootfs             = " << rootfs << endl;
    cout << "process_cwd        = " << process_cwd << endl;
    cout << "process_env        = " << process_env << endl;
    cout << "process_command    = " << process_command << endl;

    for (size_t i = 0; i < process_argv_len; i++) {
        cout << "process_argv[" << i << "]    = " << process_argv[i] << endl;
    }

    // 实际上子进程将被替换, 所以不必 return 0, 也不必 delete 什么.
    return 0; // 仅为了避免 linter 的警告
}

char **args_to_argv(json &args) {
    // ref: https://linux.die.net/man/3/execvp
    //
    // int execvp(const char *file, char *const argv[]);
    //
    // The execv(), execvp(), and execvpe() functions provide an array of
    // pointers to null-terminated strings that represent the argument list
    // available to the new program. The first argument, by convention, should
    // point to the filename associated with the file being executed. The array
    // of pointers must be terminated by a NULL pointer.

    char **result = new char *[args.size() + 1];

    for (size_t i = 0; i < args.size(); i++) {
        auto arg = args[i].get<string>();

        result[i] = new char[arg.size() + 1];
        std::copy(arg.begin(), arg.end(), result[i]);
        result[i][arg.size()] = '\0';
    }
    result[args.size()] = nullptr;

    return result;
}
