#ifndef _CHILD_PROCESS_MIDDLEWARE_HPP
#define _CHILD_PROCESS_MIDDLEWARE_HPP

#include "child_process_args.hpp"

typedef void (*Middleware)(ChildProcessArgs *args);

void RegisterMiddleware();

void ExecuteMiddleware(ChildProcessArgs *args);

void MountRootfs(ChildProcessArgs *args);

void SymlinkDefault(ChildProcessArgs *args);

void PrepareSocket(ChildProcessArgs *args);

void ConnectRuntime(ChildProcessArgs *args);

void CheckUserMapping(ChildProcessArgs *args);

void SetProcessId(ChildProcessArgs *args);

void MountDevice(ChildProcessArgs *args);

void CreateDevice(ChildProcessArgs *args);

void CreateDefautDevice(ChildProcessArgs *args);

void CheckPivot(ChildProcessArgs *args);

void PivotRootfs(ChildProcessArgs *args);

void SetHostName(ChildProcessArgs *args);

void SetEnv(ChildProcessArgs *args);

void GetReady(ChildProcessArgs *args);

void CheckStart(ChildProcessArgs *args);

void SetDir(ChildProcessArgs *args);

void SendExit(ChildProcessArgs *args);

#endif