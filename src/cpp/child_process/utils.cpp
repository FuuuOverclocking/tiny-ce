#include "utils.hpp"
#include "child_process_args.hpp"
#include <filesystem>
#include <string>
#include <unistd.h>

using std::string;
using std::filesystem::path;

/**
 * 将 rootfs 解析为绝对路径. rootfs 是相对路径时, 相对于 config_path 所在路径.
 */
string resolve_rootfs(const char *config_path, string rootfs) {
    if (rootfs.at(0) == '/') {
        return rootfs;
    }
    return path(config_path).parent_path().append(rootfs).string();
}

path get_container_socket_path(const char *sock_path) {
    path runtime_path = path(sock_path);
    path container_file = runtime_path.stem();
    container_file += "_container";
    container_file += runtime_path.extension();
    runtime_path.replace_filename(container_file);
    return runtime_path;
}

void report_error(int sock, string err) {
    if (sock != -1) {
        write(sock, err.c_str(), strlen(err.c_str()));
    }
}