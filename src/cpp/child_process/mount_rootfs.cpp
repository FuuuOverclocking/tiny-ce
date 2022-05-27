#include "hook.hpp"
#include "debug.hpp"
#include "utils.hpp"
#include "child_process_args.hpp"
#include <sys/mount.h>

using Fuu::debug, Fuu::DebugLevel;

void MountRootfs(ChildProcessArgs *args){
    auto err=mount(NULL,"/",NULL,MS_REC|MS_PRIVATE,NULL);
    assert(err==0);
    auto rootfs = args->config["root"]["path"].get<string>();
    auto resolved_rootfs=resolve_rootfs(args->config_path,rootfs);
    // auto err=mount(resolved_rootfs.c_str(),resolved_rootfs.c_str(),NULL,MS_BIND,NULL);
    // assert(err==0);
    // assert_perror(errno);
}