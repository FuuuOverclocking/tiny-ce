// 请勿在该文件 include 标准库

extern "C" void setup_args(const char *config_path, const char *init_lock_path,
                           const char *sock_path, int pty_socket);
extern "C" int child_main();
