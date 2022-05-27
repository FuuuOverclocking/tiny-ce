#ifndef _CHILD_PROCESS_UTILS_HPP
#define _CHILD_PROCESS_UTILS_HPP

#include <string>

using std::string;

string resolve_rootfs(const char *config_path, string rootfs);

#endif