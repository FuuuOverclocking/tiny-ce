#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "utils.hpp"
#include <string>
#include <sys/types.h>
#include <unistd.h>

using std::string;

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

void SetHostName(ChildProcessArgs *args) {
    auto hostname = args->config["hostname"].get<string>();
    auto err = sethostname(hostname.c_str(), strlen(hostname.c_str()));
    assert(err == 0);
}