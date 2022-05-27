#include "hook.hpp"
#include<vector>

using std::vector;

static vector<Hook> fn_array;

void RegisterHook(){
    fn_array.push_back(MountRootfs);
}

void ExecuteHook(ChildProcessArgs *args){
    for(auto fn:fn_array){
        fn(args);
    }
}