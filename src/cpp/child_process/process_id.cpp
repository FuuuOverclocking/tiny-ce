#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include <sys/types.h>
#include <unistd.h>

void SetProcessId(ChildProcessArgs *args) {
    auto process = args->config["process"];
    auto user = process["user"];
    auto uid = user["uid"].get<uid_t>();
    auto gid = user["gid"].get<uid_t>();
    auto err = setuid(uid);
    assert(err != -1);
    err = setgid(gid);
    assert(err != -1);
}