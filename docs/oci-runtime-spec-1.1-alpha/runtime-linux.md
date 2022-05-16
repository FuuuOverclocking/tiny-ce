# <a name="linuxRuntime" />Linux Runtime

## <a name="runtimeLinuxFileDescriptors" />文件描述符

默认情况下，只有 `stdin`, `stdout` 和 `stderr` 文件描述符由运行时为应用程序保持开放。
运行时可以向应用程序传递额外的文件描述符，以支持诸如[socket activation][socket-activated-containers]等功能。
一些文件描述符可能会被重定向到 `/dev/null`，尽管它们是开放的。

## <a name="runtimeLinuxDevSymbolicLinks" /> Dev 符号链接

在创建容器时（[lifecycle](runtime.md#lifecycle)中的第2步），如果源文件在处理[`mounts`](config.md#mounts)后存在，运行时必须创建以下符号链接。

|    Source       | Destination |
| --------------- | ----------- |
| /proc/self/fd   | /dev/fd     |
| /proc/self/fd/0 | /dev/stdin  |
| /proc/self/fd/1 | /dev/stdout |
| /proc/self/fd/2 | /dev/stderr |


[socket-activated-containers]: http://0pointer.de/blog/projects/socket-activated-containers.html

---

通过 www.DeepL.com/Translator（免费版）翻译, 然后手工校对。