#include "child_process_args.hpp"
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
                assert(read_bytes != -1);
                report_error(args, "error_test");
            }
            break;
        }
    }
    report_error(args, "error_test_out");
}