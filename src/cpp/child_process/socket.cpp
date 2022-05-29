#include "child_process_args.hpp"
#include "middleware.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

const int QLEN = 10;

void InitSocket(ChildProcessArgs *args) {
    assert(args->init_lock_path != nullptr);
    // init socket
    args->init_lock_sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    assert(args->init_lock_sock != -1);
    // set socket addr
    sockaddr_un c_init_un, s_init_un;
    memset(&c_init_un, 0, sizeof(c_init_un));
    memset(&s_init_un, 0, sizeof(s_init_un));
    c_init_un.sun_family = AF_UNIX;
    s_init_un.sun_family = AF_UNIX;
    // need a new socket path
    auto init_un_path = get_container_socket_path(args->init_lock_path);
    strcpy(c_init_un.sun_path, init_un_path.c_str());
    strcpy(s_init_un.sun_path, args->init_lock_path);
    int c_len = offsetof(sockaddr_un, sun_path) + strlen(c_init_un.sun_path);
    int s_len = offsetof(sockaddr_un, sun_path) + strlen(s_init_un.sun_path);
    // client bind
    auto err = bind(args->init_lock_sock, (sockaddr *)&c_init_un, c_len);
    assert(err == 0);
    // connect server
    err = connect(args->init_lock_sock, (sockaddr *)&s_init_un, s_len);
    assert(err == 0);
}

void ContainerSocket(ChildProcessArgs *args) {
    assert(args->sock_path != nullptr);
    // init socket
    args->container_sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    assert(args->container_sock != -1);
    // set socket addr
    sockaddr_un s_container_un;
    memset(&s_container_un, 0, sizeof(s_container_un));
    s_container_un.sun_family = AF_UNIX;
    auto container_un_path = get_container_socket_path(args->sock_path);
    strcpy(s_container_un.sun_path, container_un_path.c_str());
    int s_len =
        offsetof(sockaddr_un, sun_path) + strlen(s_container_un.sun_path);
    // set server
    auto err = bind(args->container_sock, (sockaddr *)&s_container_un, s_len);
    assert(err == 0);
    // listen to client
    err = listen(args->container_sock, QLEN);
    assert(err == 0);
}

void PrepareSocket(ChildProcessArgs *args) {
    InitSocket(args);
    ContainerSocket(args);
    const char *init_msg = "ok";
    auto write_bytes = write(args->init_lock_sock, init_msg, strlen(init_msg));
    assert(write_bytes != -1);
    close(args->init_lock_sock);
}

void ConnectRuntime(ChildProcessArgs *args) {
    sockaddr_un runtime_un;
    socklen_t accept_un_size = sizeof(runtime_un);
    args->container_receive_runtime_sock =
        accept(args->container_sock, (sockaddr *)&runtime_un, &accept_un_size);
    assert(args->container_receive_runtime_sock != -1);
}