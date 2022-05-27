#include "utils.hpp"
#include <string>
#include <filesystem>

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