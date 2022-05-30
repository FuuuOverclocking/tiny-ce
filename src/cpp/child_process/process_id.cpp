#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include <sys/types.h>
#include <unistd.h>

void SetProcessId(ChildProcessArgs *args) {
    auto process = args->config["process"];
    assert(!process.is_null());
    auto user = process["user"];
    assert(!user.is_null());
    assert(!(user["uid"].is_null() || user["gid"].is_null()));
    auto uid = user["uid"].get<uid_t>();
    auto gid = user["gid"].get<uid_t>();
    auto err = setuid(uid);
    assert(err != -1);
    err = setgid(gid);
    assert(err != -1);
}