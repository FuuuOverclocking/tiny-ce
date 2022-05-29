#ifndef _CHILD_PROCESS_MIDDLEWARE_HPP
#define _CHILD_PROCESS_MIDDLEWARE_HPP

#include "child_process_args.hpp"

typedef void (*Middleware)(ChildProcessArgs *args);

void RegisterMiddleware();

void ExecuteMiddleware(ChildProcessArgs *args);

void MountRootfs(ChildProcessArgs *args);

void MountDevice(ChildProcessArgs *args);

void CreateDevice(ChildProcessArgs *args);

void SymlinkDefault(ChildProcessArgs *args);

void PrepareSocket(ChildProcessArgs *args);

void ConnectRuntime(ChildProcessArgs *args);

#endif