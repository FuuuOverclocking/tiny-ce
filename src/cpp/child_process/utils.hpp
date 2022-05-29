#ifndef _CHILD_PROCESS_UTILS_HPP
#define _CHILD_PROCESS_UTILS_HPP

#include <filesystem>
#include <string>

using std::string;
using std::filesystem::path;

string resolve_rootfs(const char *config_path, string rootfs);

path get_container_socket_path(const char *sock_path);

#endif