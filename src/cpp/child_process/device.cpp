#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "utils.hpp"
#include "vendors/json.hpp"
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <unistd.h>

using Fuu::debug, Fuu::DebugLevel;
using nlohmann::json;
using std::string;

auto default_devices = R"(
    [
        {
            "path":"/dev/null",
            "type":"c",
            "major":1,
            "minor":3,
            "file_mode":"0666",
            "uid":0,
            "gid":0
        },
        {
            "path":"/dev/zero",
            "type":"c",
            "major":1,
            "minor":5,
            "file_mode":"0666",
            "uid":0,
            "gid":0
        },
        {
            "path":"/dev/full",
            "type":"c",
            "major":1,
            "minor":7,
            "file_mode":"0666",
            "uid":0,
            "gid":0
        },
        {
            "path":"/dev/random",
            "type":"c",
            "major":1,
            "minor":8,
            "file_mode":"0444",
            "uid":0,
            "gid":0
        },
        {
            "path":"/dev/urandom",
            "type":"c",
            "major":1,
            "minor":9,
            "file_mode":"0444",
            "uid":0,
            "gid":0
        },
        {
            "path":"/dev/tty",
            "type":"c",
            "major":5,
            "minor":0,
            "file_mode":"0666",
            "uid":0,
            "gid":0
        },
        {
            "path":"/dev/ptmx",
            "type":"c",
            "major":5,
            "minor":2,
            "file_mode":"0666",
            "uid":0,
            "gid":0
        }
    ]
)"_json;

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
        throw std::logic_error("非法的 mode");
    }
}

void CreateSingleDevice(json device, string rootfs) {
    auto path = rootfs + device["path"].get<string>();
    auto mask = device["file_mode"].get<string>();
    auto mode = toDeviceMode(device["type"].get<string>()[0]) |
                strtoul(mask.c_str(), NULL, 8);
    dev_t dev =
        makedev(device["major"].get<int64_t>(), device["minor"].get<int64_t>());
    auto err = mknod(path.c_str(), mode, dev);
    // debug.info("path:", path, " mode:", mode);
    // debug.info(device, "err:", errno);
    // assert_perror(errno);
    assert(err == 0);
    if (!device["uid"].is_null()) {
        err = chown(path.c_str(), device["uid"].get<uid_t>(), -1);
        assert(err == 0);
    }
    if (!device["gid"].is_null()) {
        err = chown(path.c_str(), -1, device["gid"].get<gid_t>());
        assert(err == 0);
    }
}

void BindSingleDevice(json device, string rootfs) {
    auto path = rootfs + device["path"].get<string>();
    auto source = device["path"].get<string>();
    auto err = mount(source.c_str(), path.c_str(), NULL, MS_BIND, NULL);
    // debug.info("Source:", source, " Target:", path, " err:", errno);
    // assert_perror(errno);
    assert(err == 0);
}

void CreateDevice(ChildProcessArgs *args) {
    if (args->config["linux"]["devices"].is_array()) {
        for (auto &device : args->config["linux"]["devices"]) {
            CreateSingleDevice(device, args->resolved_rootfs);
        }
    }
}

void CreateDefautDevice(ChildProcessArgs *args) {
    for (auto &device : default_devices) {
        // CreateSingleDevice(device, args->resolved_rootfs);
        BindSingleDevice(device, args->resolved_rootfs);
    }
    // report_error(args->container_receive_runtime_sock, "error_test");
}