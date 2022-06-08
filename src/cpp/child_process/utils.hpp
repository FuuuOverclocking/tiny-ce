#ifndef _CHILD_PROCESS_UTILS_HPP
#define _CHILD_PROCESS_UTILS_HPP

#include "child_process_args.hpp"
#include <filesystem>
#include <string>

std::string resolve_rootfs(const char *config_path, std::string rootfs);
std::filesystem::path get_container_socket_path(const char *sock_path);
void report_error(int sock, std::string err);

#endif