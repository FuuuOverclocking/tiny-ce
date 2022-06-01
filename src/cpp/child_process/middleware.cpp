#include "middleware.hpp"
#include <array>
// #include <vector>

using std::array;
// using std::vector;

// static vector<Middleware> fn_array;

array fn_array = {PrepareSocket, ConnectRuntime,     CheckUserMapping,
                  SetProcessId,  MountRootfs,        MountDevice,
                  CreateDevice,  CreateDefautDevice, SymlinkDefault};

// void RegisterMiddleware() {
//     fn_array.push_back(PrepareSocket);
//     fn_array.push_back(ConnectRuntime);
//     fn_array.push_back(CheckUserMapping);
//     fn_array.push_back(SetProcessId);
//     fn_array.push_back(MountRootfs);
//     fn_array.push_back(MountDevice);
//     fn_array.push_back(CreateDevice);
//     fn_array.push_back(CreateDefautDevice);
//     fn_array.push_back(SymlinkDefault);
// }

void ExecuteMiddleware(ChildProcessArgs *args) {
    for (auto fn : fn_array) {
        fn(args);
    }
}