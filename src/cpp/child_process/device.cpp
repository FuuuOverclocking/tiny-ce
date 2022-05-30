#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "utils.hpp"
#include "vendors/json.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <unistd.h>

using Fuu::debug, Fuu::DebugLevel;

mode_t toDeviceMode(char mode) {
    switch (mode) {
    case 'c':
    case 'u':
        return S_IFCHR;
    case 'b':
        return S_IFBLK;
    case 'p':
        return S_IFIFO;
    default:
        assert(1);
    }
}

void CreateSingleDevice(json device, string rootfs) {
    auto path = rootfs + device["path"].get<string>();
    auto mode = toDeviceMode(device["type"].get<string>()[0]);
    dev_t dev =
        makedev(device["major"].get<int64_t>(), device["minor"].get<int64_t>());
    auto err = mknod(path.c_str(), mode, dev);
    assert(err == 0);
}

void CreateDevice(ChildProcessArgs *args) {}