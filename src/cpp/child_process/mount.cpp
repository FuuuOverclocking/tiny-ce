#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "utils.hpp"
#include <sched.h>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

using Fuu::debug, Fuu::DebugLevel;
using std::string;

static int pivot_root(const char *new_root, const char *put_old) {
    return syscall(SYS_pivot_root, new_root, put_old);
}

void MountRootfs(ChildProcessArgs *args) {
    auto err = mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL);
    assert(err == 0);

    err = mount(args->resolved_rootfs.c_str(), args->resolved_rootfs.c_str(),
                NULL, MS_BIND | MS_REC, NULL);
    assert_perror(errno);
}

void MountDevice(ChildProcessArgs *args) {
    auto mounts = args->config["mounts"];
    if (mounts.is_array()) {
        for (auto &entry : mounts) {
            auto dest = entry["destination"].get<string>();
            dest = args->resolved_rootfs + dest;
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

void PivotRootfs(ChildProcessArgs *args) {
    auto err = chdir(args->resolved_rootfs.c_str());
    assert(err == 0);
    auto old_path = args->resolved_rootfs + "/oldrootfs";
    err = mkdir(old_path.c_str(), 0777);
    assert(err == 0);
    err = pivot_root(args->resolved_rootfs.c_str(), old_path.c_str());
    assert(err == 0);
    err = chdir("/");
    assert(err == 0);
    err = umount2("./oldrootfs", MNT_DETACH);
    assert(err == 0);
    err = rmdir("./oldrootfs");
    assert(err == 0);
}