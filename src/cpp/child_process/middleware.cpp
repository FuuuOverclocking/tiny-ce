#include "middleware.hpp"
// #include <array>
#include <vector>

using std::array;
using std::vector;

// clang-format off
array fn_array = {
    PrepareSocket,
    ConnectRuntime,
    CheckUserMapping,
    SetProcessId,
    SetNetworkNamespace,
    MountRootfs,
    MountDevice,
    CreateDevice,
    CreateDefautDevice,
    SymlinkDefault,
    CheckPivot,
    PivotRootfs,
    SetHostName,
    SetEnv,
    GetReady,
    ConnectRuntime,
    CheckStart,
    SetDir,
};
// clang-format on

void ExecuteMiddleware(ChildProcessArgs *args) {
    for (auto fn : fn_array) {
        fn(args);
    }
}