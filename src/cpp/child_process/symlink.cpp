#include "child_process_args.hpp"
#include "debug.hpp"
#include "middleware.hpp"
#include "utils.hpp"
#include <string>
#include <unistd.h>

using Fuu::debug, Fuu::DebugLevel;
using std::string;

auto default_symlinks = R"(
    [
        {
            "target":"/proc/self/fd",
            "linkpath":"/dev/fd"
        },
        {
            "target":"/proc/self/fd/0",
            "linkpath":"/dev/stdin"
        },
        {
            "target":"/proc/self/fd/1",
            "linkpath":"/dev/stdout"
        },
        {
            "target":"/proc/self/fd/2",
            "linkpath":"/dev/stderr"
        }
    ]
)"_json;

void SymlinkDefault(ChildProcessArgs *args) {
    for (auto &link : default_symlinks) {
        auto link_path = args->resolved_rootfs + link["linkpath"].get<string>();
        auto target = link["target"].get<string>();
        auto err = symlink(target.c_str(), link_path.c_str());
        assert_perror(errno);
        assert(err == 0);
    }
}