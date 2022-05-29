#include "middleware.hpp"
#include "child_process_args.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

void PrepareSocket(ChildProcessArgs *args){
    InitSocket(args);
}

void InitSocket(ChildProcessArgs *args){
    assert(args->init_lock_path!=nullptr);
    args->init_lock_sock=socket(AF_UNIX,SOCK_SEQPACKET,0);
    assert(args->init_lock_sock!=-1);
    sockaddr_un c_init_un,s_init_un;
    memset(&c_init_un,0,sizeof(c_init_un));
    memset(&s_init_un,0,sizeof(s_init_un));
    c_init_un.sun_family=AF_UNIX;
    s_init_un.sun_family=AF_UNIX;
    auto init_un_path=get_container_socket_path(args->init_lock_path);
    strcpy(c_init_un.sun_path,init_un_path.c_str());
    strcpy(s_init_un.sun_path,args->init_lock_path);
    int c_len=offsetof(sockaddr_un,sun_path)+strlen(c_init_un.sun_path);
    int s_len=offsetof(sockaddr_un,sun_path)+strlen(s_init_un.sun_path);
    auto err=bind(args->init_lock_sock,(sockaddr *)&c_init_un,c_len);
    assert(err==0);
    err=connect(args->init_lock_sock,(sockaddr *)&s_init_un,s_len);
    assert(err==0);
}