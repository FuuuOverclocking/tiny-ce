#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "utils.hpp"
#include "vendors/json.hpp"
#include <unistd.h>

using nlohmann::json;

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
            }
            break;
        }
    }
}