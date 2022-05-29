#include "middleware.hpp"
#include<vector>

using std::vector;

static vector<Middleware> fn_array;

void RegisterMiddleware(){
    fn_array.push_back(PrepareSocket);
    fn_array.push_back(MountRootfs);
    fn_array.push_back(MountDevice);
    fn_array.push_back(CreateDevice);
    fn_array.push_back(SymlinkDefault); 
}

void ExecuteMiddleware(ChildProcessArgs *args){
    for(auto fn:fn_array){
        fn(args);
    }
}