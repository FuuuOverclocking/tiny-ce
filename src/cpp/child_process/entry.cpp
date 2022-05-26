#include "entry.hpp"
#include "debug.hpp"
#include "vendors/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

using Fuu::debug, Fuu::DebugLevel, Fuu::Debug;
using nlohmann::json;
using std::ifstream;
using std::string;
using std::vector;
using std::filesystem::path;

ChildProcessArgs *args = nullptr;

void _setup_args(const char *config_path, const char *init_lock_path,
                 const char *sock_path, int pty_socket) {
    args = new ChildProcessArgs;

    ifstream(config_path) >> args->config;
    args->config_path = config_path;
    args->init_lock_path = init_lock_path;
    args->sock_path = sock_path;
    args->pty_socket = pty_socket;
}

int _child_main() {
    assert(args != nullptr);
    // debug.curr_debug_level = DebugLevel::Warn; // 令其只打印 warn, error

    debug.info("C++ 子进程开始运行...");

    auto rootfs = args->config["root"]["path"].get<string>();
    auto resolved_rootfs = resolve_rootfs(args->config_path, rootfs);

    auto process_cwd = args->config["process"]["cwd"].get<string>();

    auto process_env = args->config["process"]["env"];
    assert(process_env.is_array());

    auto process_args = args->config["process"]["args"];
    assert(process_args.is_array());

    auto process_command = process_args[0].get<string>();
    auto process_argv = args_to_argv(process_args);
    auto process_argv_len = process_args.size();

    debug.info("rootfs             = ", rootfs);
    debug.info("resolved_rootfs    = ", resolved_rootfs);
    debug.info("process.cwd        = ", process_cwd);
    debug.info("process.env        = ", process_env);
    debug.info("process.command    = ", process_command);

    for (size_t i = 0; i < process_argv_len; i++) {
        debug.info("process_argv[", i, "]    = ", process_argv[i]);
    }

    // execvp(process_command.c_str(), process_argv);
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

/**
 * 将 rootfs 解析为绝对路径. rootfs 是相对路径时, 相对于 config_path 所在路径.
 */
string resolve_rootfs(const char *config_path, string rootfs) {
    if (rootfs.at(0) == '/') {
        return rootfs;
    }
    return path(config_path).parent_path().append(rootfs).string();
}
