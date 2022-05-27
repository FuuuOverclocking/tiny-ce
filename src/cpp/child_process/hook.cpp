#include "hook.hpp"
#include<vector>

using std::vector;

static vector<Hook> fn_array;

void RegisterHook(){
    fn_array.push_back(MountRootfs);
    fn_array.push_back(MountDevice);
    fn_array.push_back(CreateDevice);
    fn_array.push_back(CreateDefaultDevice);
    fn_array.push_back(SymlinkDefault);
}

void ExecuteHook(ChildProcessArgs *args){
    for(auto fn:fn_array){
        fn(args);
    }
}