#![allow(deprecated)]

use nix::{
    sys::socket::{bind, connect, listen, socket, AddressFamily, SockAddr, SockFlag, SockType},
    unistd::{close, read, write},
};
use std::path::Path;

pub struct IpcParent {
    fd: i32,
    sock_path: String,
}
impl IpcParent {
    pub fn new(path: &String) -> IpcParent {
        let socket_raw_fd = socket(
            AddressFamily::Unix,
            SockType::SeqPacket,
            SockFlag::SOCK_CLOEXEC,
            None,
        )
        .unwrap();
        let sockaddr = SockAddr::new_unix(Path::new(path)).unwrap();
        bind(socket_raw_fd, &sockaddr).expect("无法创建 UNIX socket");
        listen(socket_raw_fd, 10).unwrap();
        IpcParent {
            fd: socket_raw_fd,
            sock_path: path.clone(),
        }
    }
    pub fn wait(&self) -> String {
        let child_socket = nix::sys::socket::accept(self.fd).unwrap();
        let mut buf = [0; 1024];
        let num = read(child_socket, &mut buf).unwrap();
        std::str::from_utf8(&buf[0..num])
            .unwrap()
            .trim()
            .to_string()
    }
    pub fn close(&self) {
        close(self.fd).unwrap();
        std::fs::remove_file(&self.sock_path).unwrap();
    }
}

pub struct IpcChild {
    fd: i32,
}
impl IpcChild {
    pub fn new(path: &String) -> IpcChild {
        let socket_raw_fd = socket(
            AddressFamily::Unix,
            SockType::SeqPacket,
            SockFlag::SOCK_CLOEXEC,
            None,
        )
        .unwrap();
        let sockaddr = SockAddr::new_unix(Path::new(path)).unwrap();
        connect(socket_raw_fd, &sockaddr).unwrap();
        IpcChild { fd: socket_raw_fd }
    }
    pub fn notify(&self, msg: &String) {
        write(self.fd, msg.as_bytes()).unwrap();
    }
    pub fn close(&self) {
        close(self.fd).unwrap();
    }
}

pub struct IpcChannel {
    fd: i32,
    sock_path: String,
    _client: Option<i32>,
}
impl IpcChannel {
    pub fn new(path: &String) -> IpcChannel {
        let socket_raw_fd = socket(
            AddressFamily::Unix,
            SockType::SeqPacket,
            SockFlag::SOCK_CLOEXEC,
            None,
        )
        .unwrap();
        let sockaddr = SockAddr::new_unix(Path::new(path)).unwrap();
        bind(socket_raw_fd, &sockaddr).unwrap();
        listen(socket_raw_fd, 10).unwrap();
        IpcChannel {
            fd: socket_raw_fd,
            sock_path: path.clone(),
            _client: None,
        }
    }

    pub fn connect(path: &String) -> IpcChannel {
        let socket_raw_fd = socket(
            AddressFamily::Unix,
            SockType::SeqPacket,
            SockFlag::SOCK_CLOEXEC,
            None,
        )
        .unwrap();
        let sockaddr = SockAddr::new_unix(Path::new(path)).unwrap();
        connect(socket_raw_fd, &sockaddr).unwrap();
        IpcChannel {
            fd: socket_raw_fd,
            sock_path: path.clone(),
            _client: None,
        }
    }

    pub fn accept(&mut self) {
        let child_socket_fd = nix::sys::socket::accept(self.fd).unwrap();
        self._client = Some(child_socket_fd);
    }

    pub fn send(&self, msg: &str) {
        let fd = match self._client {
            Some(fd) => fd,
            None => self.fd,
        };

        write(fd, msg.as_bytes()).unwrap();
    }

    pub fn recv(&self) -> String {
        let fd = match self._client {
            Some(fd) => fd,
            None => self.fd,
        };
        let mut buf = [0; 1024];
        let num = read(fd, &mut buf).unwrap();

        std::str::from_utf8(&buf[0..num])
            .unwrap()
            .trim()
            .to_string()
    }

    #[allow(dead_code)]
    pub fn send_recv(&self, msg: &str) -> String {
        self.send(msg);
        self.recv()
    }

    pub fn close(&self) {
        close(self.fd).unwrap();

        std::fs::remove_file(&self.sock_path).unwrap();
    }
}
