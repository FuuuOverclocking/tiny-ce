#ifndef _CHILD_PROCESS_HOOK_HPP
#define _CHILD_PROCESS_HOOK_HPP

#include "child_process_args.hpp"

typedef void (*Hook)(ChildProcessArgs *args);

void RegisterHook();

void ExecuteHook(ChildProcessArgs *args);

void MountRootfs(ChildProcessArgs *args);

#endif