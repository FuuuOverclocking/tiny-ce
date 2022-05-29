#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "utils.hpp"

using Fuu::debug, Fuu::DebugLevel;

// TODO: 对OCI中要求fd做映射
void SymlinkDefault(ChildProcessArgs *args) {}