#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "vendors/json.hpp"
#include <string>
#include <unistd.h>

using Fuu::debug, Fuu::DebugLevel;
using nlohmann::json;
using std::string;

const int BUF_LEN = 200;

void CheckUserMapping(ChildProcessArgs *args) {
    auto args_ns = args->config["linux"]["namespaces"];
    assert(args_ns.is_array());
    for (auto &ns : args_ns) {
        if (ns["type"].get<string>() == "user") {
            if (ns["path"].is_null()) {
                char *buf = new char[BUF_LEN];
                auto read_bytes = read(args->container_receive_runtime_sock,
                                       buf, sizeof(buf));
                expect(read_bytes != -1, "read container.sock 时发生错误");
                expect(strcmp(buf, "mapped") == 0,
                       "期望从 container.sock 接收到 mapped, 意外接收到 ", buf);
                delete buf;
            }
            break;
        }
    }
}

void CheckPivot(ChildProcessArgs *args) {
    const char *check_msg = "pivot?";
    auto write_bytes = write(args->container_receive_runtime_sock, check_msg,
                             strlen(check_msg));
    assert(write_bytes != -1);
    char *buf = new char[BUF_LEN];
    auto read_bytes =
        read(args->container_receive_runtime_sock, buf, sizeof(buf));
    assert(read_bytes != -1);
    expect(strcmp(buf, "ok") == 0,
           "期望从 container.sock 接收到 ok, 意外接收到 ", buf);
    delete buf;
}