#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "utils.hpp"
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>

using std::string;

const int BUF_SIZE = 200;

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

void SetEnv(ChildProcessArgs *args) {
    auto err = clearenv();
    assert(err == 0);
    for (auto &env : args->config["process"]["env"]) {
        char *buf = new char[BUF_SIZE];
        strcpy(buf, env.get<string>().c_str());
        err = putenv(buf);
        assert(err == 0);
    }
}