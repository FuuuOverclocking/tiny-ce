#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "utils.hpp"
#include "vendors/json.hpp"
#include <fcntl.h>
#include <sched.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

using Fuu::debug, Fuu::DebugLevel;
using std::string;

void SetNetworkNamespace(ChildProcessArgs *args) {
    auto namespaces = args->config["linux"]["namespaces"];
    if (namespaces.is_null()) {
        return;
    }
    for (auto &ce_namespace : namespaces) {
        if (ce_namespace["type"].get<string>() != "network") {
            continue;
        }
        if (ce_namespace["path"].is_null()) {
            return;
        }
        auto path = ce_namespace["path"].get<string>();
        auto fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
        auto err = setns(fd, CLONE_NEWNET);
        assert(err == 0);
    }
}