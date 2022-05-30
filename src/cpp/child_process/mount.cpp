#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "utils.hpp"
#include <sched.h>
#include <sys/mount.h>
#include <unistd.h>

using Fuu::debug, Fuu::DebugLevel;

void MountRootfs(ChildProcessArgs *args) {
    auto err = mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL);
    assert(err == 0);
    auto rootfs = args->config["root"]["path"].get<string>();
    auto resolved_rootfs = resolve_rootfs(args->config_path, rootfs);

    // err = mount(resolved_rootfs.c_str(), resolved_rootfs.c_str(), NULL,
    // MS_BIND,
    //             NULL);
    // assert(err==0);
    // assert_perror(errno);
    // report_error(args->container_receive_runtime_sock, "error_test");
}

void MountDevice(ChildProcessArgs *args) {
    auto mounts = args->config["mounts"];
    if (mounts.is_array()) {
        for (auto &entry : mounts) {
            assert(!entry["destination"].is_null());
            auto dest = entry["destination"].get<string>();
            unsigned long flag = 0;
            if (entry["type"].get<string>() == "bind") {
                flag |= MS_BIND;
            }
            auto err = mount(entry["source"].is_null()
                                 ? NULL
                                 : entry["source"].get<string>().c_str(),
                             dest.c_str(), NULL, flag, NULL);
            assert(err == 0);
        }
    }
}