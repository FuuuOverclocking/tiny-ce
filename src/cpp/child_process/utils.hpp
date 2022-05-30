#ifndef _CHILD_PROCESS_UTILS_HPP
#define _CHILD_PROCESS_UTILS_HPP

#include "child_process_args.hpp"
#include <filesystem>
#include <string>

using std::string;
using std::filesystem::path;

string resolve_rootfs(const char *config_path, string rootfs);

path get_container_socket_path(const char *sock_path);

void report_error(int sock, string err);

#endif