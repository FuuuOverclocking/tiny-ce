# <a name="configuration" />Configuration

这个配置文件包含了对容器实施 [标准操作](runtime.md#operations) 所需的元数据。
这包括要运行的进程、要注入的环境变量、要使用的沙箱功能等等。

规范模式在本文档中定义，但在 [`schema/config-schema.json`](schema/config-schema.json) 中有一个JSON Schema，在[`specs-go/config.go`](specs-go/config.go)中有 Go bindings 。
[平台](spec.md#platforms)的特定配置模式在下面链接的[平台特定文件](#platform-specific-configuration)中定义。
对于只为某些[平台](spec.md#platforms)定义的属性，Go属性有一个`平台`标签，列出这些协议（例如`平台: "linux,solaris"`）。

下面是对配置格式中定义的每个字段的详细描述，并指定了有效值。
特定于平台的字段也是这样标识的。
对于所有平台特定的配置值，下面[平台特定配置](#platform-specific-configuration)部分定义的范围适用。


## <a name="configSpecificationVersion" />配置版本

- **`ociVersion`** (string, REQUIRED) 必须是 [SemVer v2.0.0][semver-v2.0.0] 格式，并指定捆绑包符合的开放容器倡议运行时规范的版本。
    Open Container Initiative Runtime Specification 遵循语义上的版本划分，并在 major 版本中保留前向和后向兼容性。
    例如，如果一个配置符合本规范的 1.1 版本，它就与支持本规范的任何1.1或更高版本的所有运行时兼容，但与支持1.0而不支持1.1的运行时不兼容。

### 示例

```json
"ociVersion": "0.1.0"
```

## <a name="configRoot" />Root

**`root`** (object, OPTIONAL) 指定容器的根文件系统。
在 Windows 上，对于 Windows Server 容器，这个字段是必须的。
对于 [Hyper-V Containers](config-windows.md#hyperv)，这个字段必须不被设置。

在所有其他平台上，这个字段是必须的。

- **`path`** (string, REQUIRED) 指定容器的根文件系统的路径。
    * 在 Windows 上，`path`必须是一个[卷GUID路径][naming-a-volume]。
    * 在 POSIX 平台上，`path`是一个绝对路径或一个相对路径，用于指向捆绑包。
        例如，捆绑在`/to/bundle`，根文件系统在`/to/bundle/rootfs`，`path`值可以是`/to/bundle/rootfs`或`rootfs`。
        这个值应该是传统的`rootfs'。

    一个目录必须存在于该字段所声明的路径上。

- **`readonly`** (bool, OPTIONAL) 如果为 true，那么根文件系统在容器中必须是只读的，默认为 false。
    * 在Windows上，这个字段必须是省略的或假的。

### 例子 (POSIX平台)

```json
"root": {
    "path": "rootfs",
    "readonly": true
}
```

### 示例 (Windows)

```json
"root": {
    "path": "\\\\?\\Volume{ec84d99e-3f02-11e7-ac6c-00155d7682cf}\\"
}
```

## <a name="configMounts" />Mounts

**`mounts`** (object[]，OPTIONAL) 指定 [`root`](#root)之外的额外挂载。
运行时必须按照列出的顺序挂载条目。
对于 Linux，参数与[mount(2)][mount.2]系统调用手册中记载的一样。
对于 Solaris，挂载条目对应于[zonecfg(1M)][zonecfg.1m]手册页中的'fs'资源。

- **`destination`** (string, REQUIRED) 挂载点的目的地：容器内的路径。
    这个值必须是一个绝对路径。
    * Windows：一个挂载点不能嵌套在另一个挂载点中（例如，c:\foo和c:\foo\bar）。
    * Solaris：对应于[zonecfg(1M)][zonecfg.1m]中fs资源的 "dir"。
- **`source`** (string, OPTIONAL) 一个设备名，但也可以是绑定挂载的文件或目录名，或一个假的。
    绑定挂载的路径值可以是绝对的，也可以是相对于捆绑的。
    如果选项中有`bind'或`rbind'，那么该挂载就是绑定挂载。
    * Windows：容器主机的文件系统上的一个本地目录。不支持UNC路径和映射的驱动器。
    * Solaris：对应于[zonecfg(1M)][zonecfg.1m]中fs资源的 "特殊"。
- **`options`** （string[]，OPTIONAL）要使用的文件系统的挂载选项。
    * Linux：支持的选项在[mount(8)][mount.8] man page中列出。
    注意 [filesystem-independent][mount.8-filesystem-independent] 和 [filesystem-specific][mount.8-filesystem-specific] 选项都被列出。
    * Solaris：对应于[zonecfg(1M)][zonecfg.1m]中fs资源的 "选项"。
    * Windows: 运行时必须支持 "ro"，当给出 "ro "时，挂载文件系统为只读。

### 例子 (Windows)

```json
"mounts": [
    {
        "destination": "C:\\folder-inside-container",
        "source": "C:\\folder-on-host",
        "options": ["ro"]
    }
]
```

### <a name="configPOSIXMounts" />POSIX-platform Mounts

对于 POSIX 平台，`mounts`结构有以下字段。

- **`type`** (string, OPTIONAL) 要挂载的文件系统的类型。
    * Linux：内核支持的文件系统类型，在 `/proc/filesystems` 中列出（例如，"minix", "ext2", "ext3", "jfs", "xfs", "reiserfs", "msdos", "proc", "nfs", "iso9660"）。对于绑定挂载（当`options`包括`bind`或`rbind`时），类型是无用的，通常是 "none"（未在 `/proc/filesystems` 中列出）。
    * Solaris: 对应于[zonecfg(1M)][zonecfg.1M]中fs资源的 "类型"。

### 示例 (Linux)

```json
"mounts": [
    {
        "destination": "/tmp",
        "type": "tmpfs",
        "source": "tmpfs",
        "options": ["nosuid","strictatime","mode=755","size=65536k"]
    },
    {
        "destination": "/data",
        "type": "none",
        "source": "/volumes/testing",
        "options": ["rbind","rw"]
    }
]
```

### 示例 (Solaris)

```json
"mounts": [
    {
        "destination": "/opt/local",
        "type": "lofs",
        "source": "/usr/local",
        "options": ["ro","nodevices"]
    },
    {
        "destination": "/opt/sfw",
        "type": "lofs",
        "source": "/opt/sfw"
    }
]
```

## <a name="configProcess" />Process

**`process`** (object, OPTIONAL) 指定容器的进程。
这个属性在调用 [`start`](runtime.md#start) 时是必须的。

- **`terminal`** (bool, OPTIONAL) 指定是否有一个终端连接到进程，默认为 false。
    作为一个例子，如果在 Linux 上设置为 true，将为进程分配一个伪终端对，伪终端 pty 将在进程的[标准流][stdin.3]上重复。
- **`consoleSize`** (object, OPTIONAL) 指定终端的控制台大小，以字符为单位。
    如果`terminal`是`false`或未设置，运行时必须忽略`consoleSize`。
    **`高度`** (uint, 必需的)
    **`宽度`** (uint, REQUIRED)
- **`cwd`** (string, REQUIRED) 是将为可执行文件设置的工作目录。
    这个值必须是一个绝对路径。
- **`env`** （字符串数组，可选）与[IEEE Std 1003.1-2008的`environ`][ieee-1003.1-2008-xbd-c8.1]具有相同的语义。
- **`args`**（字符串数组，OPTIONAL），与[IEEE Std 1003.1-2008 `execvp`的*argv*][ieee-1003.1-2008-function-exec]的语义类似。
    本规范扩展了IEEE标准，至少有一个条目是 REQUIRED（非Windows），该条目的语义与 `execvp` 的 *file* 相同。这个字段在 Windows 上是可选的，如果省略这个字段，`commandLine` 是必须的。
- **`commandLine`** (string, OPTIONAL) 指定在 Windows 下执行的完整命令行。
    这是在Windows上提供命令行的首选方法。如果省略，运行时将退回到转义和连接`args`的字段，然后再向Windows进行系统调用。


### <a name="configPOSIXProcess" />POSIX进程

对于支持POSIX rlimits的系统（例如Linux和Solaris），`process`对象支持以下进程的特定属性。

- **`rlimits`**（对象数组，可选）允许为进程设置资源限制。
    每个条目有以下结构。
    - **`type`** (string, REQUIRED) 被限制的平台资源。
        * Linux：有效值在[`getrlimit(2)`][getrlimit.2] man页中定义，例如`RLIMIT_MSGQUEUE`。
        * Solaris：有效值在[`getrlimit(3)`][getrlimit.3]手册页中定义，如`RLIMIT_CORE`。

        对于任何不能映射到相关内核接口的值，运行时必须[产生一个错误](runtime.md#errors)。
        对于`rlimits`中的每个条目，在`type`上进行的[`getrlimit(3)`][getrlimit.3]必须成功。
        对于以下属性，`rlim`指的是由`getrlimit(3)`调用返回的状态。

    - **`soft`** (uint64, REQUIRED) 对应资源执行的限制值。
        `rlim.rlim_cur`必须与配置的值匹配。
    - **`hard`** (uint64, REQUIRED) 非特权进程可以设置的软限制的上限。
        `rlim.rlim_max`必须与配置的值匹配。
        只有特权进程（例如具有 "CAP_SYS_RESOURCE "能力的进程）可以提高硬限制。

    如果`rlimits`包含相同`类型'的重复条目，运行时必须 [产生一个错误](runtime.md#errors)。

### <a name="configLinuxProcess" />Linux进程

对于基于 Linux 的系统，`process` 对象支持以下进程的特定属性。

- **`apparmorProfile`** (string, OPTIONAL) 指定进程的AppArmor配置文件的名称。
    有关 AppArmor 的更多信息，请参阅 [AppArmor documentation][apparmor]。
- **`capabilities`** (object, OPTIONAL) 是一个包含数组的对象，指定进程的能力集。
    有效的值在 [capabilities(7)][capabilities.7] man page 中定义，例如 `CAP_CHOWN`。
    任何不能被映射到相关的内核接口的值，或者不能被授予
    的值必须被运行时[记录为警告]（runtime.md#warnings）。
    运行时[logged as a warning](runtime.md#warnings)。如果容器配置要求的功能不能被授予，运行时不应该失败。
    容器配置要求的功能不能被授予，例如，如果运行时是在一个有一套有限的限制性环境中运行的
    例如，如果运行时在一个能力有限的环境中运行，就不会失败。
    `capabilities`包含以下属性。

    - **`effective`** （string[]，OPTIONAL） `effective`字段是为进程保留的有效能力数组。
    - **`bounding`** (string[], OPTIONAL) `bounding`字段是为进程保留的边界能力数组。
    - **`inheritable`** (string[], OPTIONAL) `inheritable`字段是为进程保留的可继承能力数组。
    - **`permitted`** (string[]，OPTIONAL) `permitted`字段是为进程保留的允许能力数组。
    - **`ambient`** (string[]，OPTIONAL) `ambient`字段是为进程保留的环境能力数组。
- **`noNewPrivileges`** (bool, OPTIONAL) 将`noNewPrivileges`设置为真，可以防止进程获得额外的权限。
    作为一个例子，内核文档中的[no_new_privs][no-new-privs]文章有关于如何在Linux上使用`prctl'系统调用实现的信息。
- **`oomScoreAdj`** *(int, OPTIONAL)* 调整[proc伪文件系统][proc_2]中进程的`[pid]/oom_score_adj`中的om-killer分数。
    如果`oomScoreAdj`被设置，运行时必须将`oom_score_adj`设置为给定值。
    如果`oomScoreAdj'没有被设置，运行时必须不改变`oom_score_adj'的值。

    这是每进程的设置，而[`disableOOMKiller`](config-linux.md#memory)是针对内存cgroup的范围。
    关于这两个设置如何一起工作的更多信息，见[内存组文档第10节。 OOM控制][cgroup-v1-memory_2]。
- **`selinuxLabel`** (string, OPTIONAL) 指定进程的SELinux标签。
    关于SELinux的更多信息，见[SELinux文档][selinux]。

### <a name="configUser" />User

进程的用户是一个特定的平台结构，允许具体控制进程以哪个用户的身份运行。

#### <a name="configPOSIXUser" />POSIX-平台用户

对于 POSIX 平台，`user` 结构有以下字段。

- **`uid`** (int, REQUIRED) 指定[容器命名空间](glossary.md#container-namespace) 中的用户ID。
- **`gid`** (int, REQUIRED) 指定[容器命名空间](glossary.md#container-namespace) 中的组ID。
- **`umask`** (int, OPTIONAL) 指定用户的[umask][umask_2]。如果没有指定，则不应从调用进程的umask中改变umask。
- **`additionalGids`** (int[], OPTIONAL) 指定要添加到进程中的 [container namespace](glossary.md#container-namespace) 的额外组ID。

注意：uid和gid的符号名称，如uname和gname分别留给上层来推导（即`/etc/passwd`解析，NSS等）

### 示例 (Linux)

```json
"process": {
    "terminal": true,
    "consoleSize": {
        "height": 25,
        "width": 80
    },
    "user": {
        "uid": 1,
        "gid": 1,
        "umask": 63,
        "additionalGids": [5, 6]
    },
    "env": [
        "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin",
        "TERM=xterm"
    ],
    "cwd": "/root",
    "args": [
        "sh"
    ],
    "apparmorProfile": "acme_secure_profile",
    "selinuxLabel": "system_u:system_r:svirt_lxc_net_t:s0:c124,c675",
    "noNewPrivileges": true,
    "capabilities": {
        "bounding": [
            "CAP_AUDIT_WRITE",
            "CAP_KILL",
            "CAP_NET_BIND_SERVICE"
        ],
       "permitted": [
            "CAP_AUDIT_WRITE",
            "CAP_KILL",
            "CAP_NET_BIND_SERVICE"
        ],
       "inheritable": [
            "CAP_AUDIT_WRITE",
            "CAP_KILL",
            "CAP_NET_BIND_SERVICE"
        ],
        "effective": [
            "CAP_AUDIT_WRITE",
            "CAP_KILL"
        ],
        "ambient": [
            "CAP_NET_BIND_SERVICE"
        ]
    },
    "rlimits": [
        {
            "type": "RLIMIT_NOFILE",
            "hard": 1024,
            "soft": 1024
        }
    ]
}
```

### 示例 (Solaris)

```json
"process": {
    "terminal": true,
    "consoleSize": {
        "height": 25,
        "width": 80
    },
    "user": {
        "uid": 1,
        "gid": 1,
        "umask": 7,
        "additionalGids": [2, 8]
    },
    "env": [
        "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin",
        "TERM=xterm"
    ],
    "cwd": "/root",
    "args": [
        "/usr/bin/bash"
    ]
}
```

#### <a name="configWindowsUser" />Windows User

对于基于 Windows 的系统，用户结构有以下字段。

- **`username`** (string, OPTIONAL) 指定进程的用户名称。

### 例子 (Windows)

```json
"process": {
    "terminal": true,
    "user": {
        "username": "containeradministrator"
    },
    "env": [
        "VARIABLE=1"
    ],
    "cwd": "c:\\foo",
    "args": [
        "someapp.exe",
    ]
}
```

## <a name="configHostname" />Hostname

- **`hostname`** (string, OPTIONAL) 指定在容器内运行的进程所看到的容器的主机名。
    例如，在Linux上，这将改变 [容器](glossary.md#container-namespace) 的 [UTS namespace][uts-namespace.7] 中的主机名。
    根据你的 [命名空间配置](config-linux.md#namespaces)，容器的 UTS 命名空间可能是 [运行时]( glossary.md#runtime-namespace) [UTS namespace][uts-namespace.7]。

### 例子

```json
"hostname": "mrsdalloway"
```

## <a name="configPlatformSpecificConfiguration" />特定平台配置

- **`linux`** (object, OPTIONAL) [Linux 专用配置](config-linux.md)。
    如果本规范的目标平台是 "linux"，则可以设置该配置。
- **`windows`** (object, OPTIONAL) [Windows专用配置](config-windows.md)。
    如果本规范的目标平台是 "windows"，则必须设置该配置。
- **`solaris`** (object, OPTIONAL) [Solaris专用配置](config-solaris.md)。
    如果本规范的目标平台是 "solaris"，则可以设置该配置。
- **`vm`** (object, OPTIONAL) [虚拟机特定配置](config-vm.md)。
    如果本规范的目标平台和架构支持硬件虚拟化，则可以设置该配置。
- **`zos`** (object, OPTIONAL) [z/OS专用配置](config-zos.md)。
    如果本规范的目标平台是`zos'，则可以设置。

### 示例 (Linux)

```json
{
    "linux": {
        "namespaces": [
            {
                "type": "pid"
            }
        ]
    }
}
```

## <a name="configHooks" />POSIX-platform Hooks

对于POSIX平台，配置结构支持 "钩子"，用于配置与容器的[生命周期](runtime.md#lifecycle)相关的自定义动作。

- **`hooks`** (object, OPTIONAL) 可能包含以下任何属性。
    - **`prestart`** (数组对象, OPTIONAL, **DEPRECATED**) 是一个[`prestart`钩子](#prestart)的数组。
        * 该数组中的条目包含以下属性。
            - **`path`** (string, REQUIRED)与[IEEE Std 1003.1-2008 `execv`的*path*][ieee-1003.1-2008-functions-exec]有类似的语义。
                本规范扩展了IEEE标准，即**`path`**必须是绝对的。
            - **`args`**（字符串数组，可选），与[IEEE Std 1003.1-2008 `execv`的*argv*][ieee-1003.1-2008-function-exec]的语义相同。
            - **`env`** （字符串数组，可选），与[IEEE标准1003.1-2008的`environ`][ieee-1003.1-2008-xbd-c8.1]有相同的语义。
            - **`timeout`** (int, OPTIONAL) 是终止钩子前的秒数。
                如果设置，`timeout`必须大于零。
        * `path`的值必须在[运行时命名空间]（glossary.md#runtime-namespace）中解析。
        * `prestart`钩子必须在[运行时命名空间](glossary.md#runtime-namespace)中执行。
    - **`createRuntime`** (array of objects, OPTIONAL) 是一个[`createRuntime`钩子](#createRuntime-hooks) 的数组。
        * 数组中的条目包含以下属性（这些条目与废弃的`prestart`钩子中的条目相同）。
            - **`path`** (string, REQUIRED)，与[IEEE Std 1003.1-2008 `execv`的*path*][ieee-1003.1-2008-function-exec]的语义相似。
                本规范扩展了IEEE标准，即**`path`**必须是绝对的。
            - **`args`**（字符串数组，可选），与[IEEE Std 1003.1-2008 `execv`的*argv*][ieee-1003.1-2008-function-exec]的语义相同。
            - **`env`** （字符串数组，可选），与[IEEE标准1003.1-2008的`environ`][ieee-1003.1-2008-xbd-c8.1]有相同的语义。
            - **`timeout`** (int, OPTIONAL) 是终止钩子前的秒数。
                如果设置，`timeout`必须大于零。
        * `path`的值必须在[运行时命名空间]（glossary.md#runtime-namespace）中解析。
        * `createRuntime`钩子必须在[运行时命名空间](glossary.md#runtime-namespace)中执行。
    - **`createContainer`** (array of objects, OPTIONAL) 是一个[`createContainer`hooks](#createContainer-hooks)的数组。
        * 数组中的条目与`createRuntime`条目具有相同的模式。
        * `path`的值必须在[运行时命名空间](glossary.md#runtime-namespace)中解析。
        * `createContainer`钩子必须在[容器命名空间](glossary.md#container-namespace)中执行。
    - **`startContainer`**（对象数组，可选）是一个[`startContainer`钩子]（#startContainer-hooks）的数组。
        * 数组中的条目与`createRuntime`条目具有相同的模式。
        * `path`的值必须在[容器命名空间](glossary.md#container-namespace)中解析。
        * `startContainer`钩子必须在[容器命名空间](glossary.md#container-namespace)中执行。
    - **`poststart`** (数组对象, OPTIONAL) 是一个[`poststart`钩子](#poststart)的数组。
        *该数组中的条目与`createRuntime`条目具有相同的模式。
        * `path`的值必须在[运行时命名空间](glossary.md#runtime-namespace)中解析。
        * `poststart`钩子必须在[运行时命名空间](glossary.md#runtime-namespace)中执行。
    - **`poststop`** (数组对象, OPTIONAL) 是一个[`poststop`钩子](#poststop) 的数组。
        *该数组中的条目与`createRuntime`条目具有相同的模式。
        * `path`的值必须在[运行时命名空间](glossary.md#runtime-namespace)中解析。
        * `poststop`钩子必须在[运行时命名空间](glossary.md#runtime-namespace)中执行。

钩子允许用户指定在各种生命周期事件之前或之后运行的程序。
钩子必须按照列出的顺序来调用。
容器的 [state](runtime.md#state) 必须通过 stdin 传递给钩子，以便它们可以做适合容器当前状态的工作。

### <a name="configHooksPrestart" />Prestart

`prestart`钩子必须在调用[`start`](runtime.md#start)操作之后，但[在执行用户指定的程序命令之前](runtime.md#lifecycle)被调用。
例如，在Linux上，它们在容器命名空间创建后被调用，所以它们提供了一个定制容器的机会（例如，网络命名空间可以在这个钩子中指定）。

注意：`prestart` 钩子已被废弃，取而代之的是 `createRuntime`、`createContainer` 和`startContainer` 钩子，它们允许在创建和启动阶段进行更细致的钩子控制。

`prestart`钩子的路径必须在 [运行时命名空间](glossary.md#runtime-namespac) 中解析。
`prestart`钩子必须在 [运行时命名空间](glossary.md#runtime-namespace) 中执行。

### <a name="configHooksCreateRuntime" />CreateRuntime 钩子

`createRuntime`钩子必须作为[`create`](runtime.md#create)操作的一部分，在运行环境创建之后（根据config.json中的配置），但在`pivot_root`或任何同等操作被执行之前被调用。

`createRuntime`钩子的路径必须在[运行时命名空间]（ glossary.md#runtime-namespace）中解析。
`createRuntime`钩子必须在[运行时名称空间](glossary.md#runtime-namespace)内执行。

例如，在Linux上，它们在容器命名空间创建后被调用，所以它们提供了一个自定义容器的机会（例如，网络命名空间可以在这个钩子中指定）。

目前对 "创建运行时 "钩子的定义还不明确，钩子作者应该只期望从运行时那里创建装载命名空间并执行装载操作。其他操作，如cgroups和SELinux/AppArmor标签可能没有被运行时执行。

注意：`runc`最初实现的`重启'钩子与规范相反，即作为`创建'操作的一部分（而不是在`启动'操作期间）。这个不正确的实现实际上对应于`createRuntime`钩子。对于将已废弃的`restart`钩子作为`createRuntime`钩子来实现的运行时，`createRuntime`钩子必须在`restart`钩子之后调用。

### <a name="configHooksCreateContainer" />CreateContainer 钩子

`createContainer`钩子必须作为[`create`](runtime.md#create)操作的一部分，在运行环境创建之后（根据config.json中的配置），但在`pivot_root`或任何同等操作被执行之前被调用。
`createContainer`钩子必须在`createRuntime`钩子之后调用。

`createContainer`钩子的路径必须在[运行时命名空间](glossary.md#runtime-namespace)中解析。
`createContainer`钩子必须在[容器命名空间](glossary.md#container-namespace)中执行。

例如，在Linux上，这将发生在`pivot_root`操作执行之前，但在mount命名空间被创建和设置之后。

目前对`createContainer`钩子的定义还不明确，钩子的作者，应该只从运行时期待挂载命名空间和不同的挂载将被设置。其他的操作，例如 cgroups 和 SELinux/AppArmor 标签，可能没有被运行时执行。

### <a name="configHooksStartContainer" />StartContainer Hooks

`startContainer`钩子必须在[用户指定的进程被执行之前](runtime.md#lifecycle)被调用，作为[`start`](runtime.md#start) 操作的一部分。
这个钩子可以用来在容器中执行一些操作，例如在容器进程产生之前在linux上运行`ldconfig`二进制文件。

`startContainer`钩子的路径必须在[容器命名空间]（ glossary.md#container-namespace）中解析。
`startContainer`钩子必须在[容器命名空间](glossary.md#container-namespace)中执行。

### <a name="configHooksPoststart" />Poststart

`poststart`钩子必须在 [用户指定的进程执行之后](runtime.md#lifecycle) 但在 [`start`](runtime.md#start) 操作返回之前被调用。
例如，这个钩子可以通知用户容器进程被催生了。

`poststart`钩子的路径必须在 [runtime namespace]( glossary.md#runtime-namespace) 中解析。
`poststart`钩子必须在 [runtime namespace](glossary.md#runtime-namespace) 中执行。

### <a name="configHooksPoststop" />Poststop

`poststop`钩子必须在 [容器被删除之后](runtime.md#lifecycle) 但在 [`delete`](runtime.md#delete) 操作返回之前被调用。
清理或调试函数就是这种钩子的例子。

`poststop`钩子的路径必须在[runtime namespace]( glossary.md#runtime-namespace)中解析。
`poststop`钩子必须在[运行时命名空间]( glossary.md#runtime-namespace)中执行。

### 摘要

关于钩子的摘要以及何时被调用，请看下表。

| Name                    | Namespace | When                                                                                                                               |
| ----------------------- | --------- | ---------------------------------------------------------------------------------------------------------------------------------- |
| `prestart` (Deprecated) | runtime   | After the start  operation is called but before the user-specified program command is executed.                                    |
| `createRuntime`         | runtime   | During the create operation, after the runtime environment has been created and before the pivot root or any equivalent operation. |
| `createContainer`       | container | During the create operation, after the runtime environment has been created and before the pivot root or any equivalent operation. |
| `startContainer`        | container | After the start operation is called but before the user-specified program command is executed.                                     |
| `poststart`             | runtime   | After the user-specified process is executed but before the start operation returns.                                               |
| `poststop`              | runtime   | After the container is deleted but before the delete operation returns.                                                            |

### Example

```json
"hooks": {
    "prestart": [
        {
            "path": "/usr/bin/fix-mounts",
            "args": ["fix-mounts", "arg1", "arg2"],
            "env":  [ "key1=value1"]
        },
        {
            "path": "/usr/bin/setup-network"
        }
    ],
    "createRuntime": [
        {
            "path": "/usr/bin/fix-mounts",
            "args": ["fix-mounts", "arg1", "arg2"],
            "env":  [ "key1=value1"]
        },
        {
            "path": "/usr/bin/setup-network"
        }
    ],
    "createContainer": [
        {
            "path": "/usr/bin/mount-hook",
            "args": ["-mount", "arg1", "arg2"],
            "env":  [ "key1=value1"]
        }
    ],
    "startContainer": [
        {
            "path": "/usr/bin/refresh-ldcache"
        }
    ],
    "poststart": [
        {
            "path": "/usr/bin/notify-start",
            "timeout": 5
        }
    ],
    "poststop": [
        {
            "path": "/usr/sbin/cleanup.sh",
            "args": ["cleanup.sh", "-f"]
        }
    ]
}
```

## <a name="configAnnotations" />Annotations

**`annotations`** (object, OPTIONAL) 包含容器的任意元数据。
这些信息可以是结构化的，也可以是非结构化的。
注释必须是一个键值映射。
如果没有注释，那么这个属性可能不存在，也可能是一个空的地图。

键必须是字符串。
键值不能是空字符串。
键值应使用反向域符号命名 - 例如：`com.example.myKey`。
使用 "org.opencontainers "命名空间的键被保留，并且不得被后续规范使用。
运行时必须像处理其他[未知属性]一样处理未知的注释键（#extensibility）。

值必须是字符串。
值可以是一个空字符串。

```json
"annotations": {
    "com.example.gpu-cores": "2"
}
```

## <a name="configExtensibility" />Extensibility

运行时可能 [log](runtime.md#warnings) 未知属性，但必须忽略它们。
这包括在遇到未知属性时不[产生错误](runtime.md#errors) 。

## 有效值

当遇到无效或不支持的值时，运行时必须生成一个错误。
除非对有效值的支持是明确要求的，否则运行时可以选择它将支持的有效值的子集。

## 配置模式示例

这里有一个完整的`config.json`例子供参考。

```json
{
    "ociVersion": "1.0.1",
    "process": {
        "terminal": true,
        "user": {
            "uid": 1,
            "gid": 1,
            "additionalGids": [
                5,
                6
            ]
        },
        "args": [
            "sh"
        ],
        "env": [
            "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin",
            "TERM=xterm"
        ],
        "cwd": "/",
        "capabilities": {
            "bounding": [
                "CAP_AUDIT_WRITE",
                "CAP_KILL",
                "CAP_NET_BIND_SERVICE"
            ],
            "permitted": [
                "CAP_AUDIT_WRITE",
                "CAP_KILL",
                "CAP_NET_BIND_SERVICE"
            ],
            "inheritable": [
                "CAP_AUDIT_WRITE",
                "CAP_KILL",
                "CAP_NET_BIND_SERVICE"
            ],
            "effective": [
                "CAP_AUDIT_WRITE",
                "CAP_KILL"
            ],
            "ambient": [
                "CAP_NET_BIND_SERVICE"
            ]
        },
        "rlimits": [
            {
                "type": "RLIMIT_CORE",
                "hard": 1024,
                "soft": 1024
            },
            {
                "type": "RLIMIT_NOFILE",
                "hard": 1024,
                "soft": 1024
            }
        ],
        "apparmorProfile": "acme_secure_profile",
        "oomScoreAdj": 100,
        "selinuxLabel": "system_u:system_r:svirt_lxc_net_t:s0:c124,c675",
        "noNewPrivileges": true
    },
    "root": {
        "path": "rootfs",
        "readonly": true
    },
    "hostname": "slartibartfast",
    "mounts": [
        {
            "destination": "/proc",
            "type": "proc",
            "source": "proc"
        },
        {
            "destination": "/dev",
            "type": "tmpfs",
            "source": "tmpfs",
            "options": [
                "nosuid",
                "strictatime",
                "mode=755",
                "size=65536k"
            ]
        },
        {
            "destination": "/dev/pts",
            "type": "devpts",
            "source": "devpts",
            "options": [
                "nosuid",
                "noexec",
                "newinstance",
                "ptmxmode=0666",
                "mode=0620",
                "gid=5"
            ]
        },
        {
            "destination": "/dev/shm",
            "type": "tmpfs",
            "source": "shm",
            "options": [
                "nosuid",
                "noexec",
                "nodev",
                "mode=1777",
                "size=65536k"
            ]
        },
        {
            "destination": "/dev/mqueue",
            "type": "mqueue",
            "source": "mqueue",
            "options": [
                "nosuid",
                "noexec",
                "nodev"
            ]
        },
        {
            "destination": "/sys",
            "type": "sysfs",
            "source": "sysfs",
            "options": [
                "nosuid",
                "noexec",
                "nodev"
            ]
        },
        {
            "destination": "/sys/fs/cgroup",
            "type": "cgroup",
            "source": "cgroup",
            "options": [
                "nosuid",
                "noexec",
                "nodev",
                "relatime",
                "ro"
            ]
        }
    ],
    "hooks": {
        "prestart": [
            {
                "path": "/usr/bin/fix-mounts",
                "args": [
                    "fix-mounts",
                    "arg1",
                    "arg2"
                ],
                "env": [
                    "key1=value1"
                ]
            },
            {
                "path": "/usr/bin/setup-network"
            }
        ],
        "poststart": [
            {
                "path": "/usr/bin/notify-start",
                "timeout": 5
            }
        ],
        "poststop": [
            {
                "path": "/usr/sbin/cleanup.sh",
                "args": [
                    "cleanup.sh",
                    "-f"
                ]
            }
        ]
    },
    "linux": {
        "devices": [
            {
                "path": "/dev/fuse",
                "type": "c",
                "major": 10,
                "minor": 229,
                "fileMode": 438,
                "uid": 0,
                "gid": 0
            },
            {
                "path": "/dev/sda",
                "type": "b",
                "major": 8,
                "minor": 0,
                "fileMode": 432,
                "uid": 0,
                "gid": 0
            }
        ],
        "uidMappings": [
            {
                "containerID": 0,
                "hostID": 1000,
                "size": 32000
            }
        ],
        "gidMappings": [
            {
                "containerID": 0,
                "hostID": 1000,
                "size": 32000
            }
        ],
        "sysctl": {
            "net.ipv4.ip_forward": "1",
            "net.core.somaxconn": "256"
        },
        "cgroupsPath": "/myRuntime/myContainer",
        "resources": {
            "network": {
                "classID": 1048577,
                "priorities": [
                    {
                        "name": "eth0",
                        "priority": 500
                    },
                    {
                        "name": "eth1",
                        "priority": 1000
                    }
                ]
            },
            "pids": {
                "limit": 32771
            },
            "hugepageLimits": [
                {
                    "pageSize": "2MB",
                    "limit": 9223372036854772000
                },
                {
                    "pageSize": "64KB",
                    "limit": 1000000
                }
            ],
            "memory": {
                "limit": 536870912,
                "reservation": 536870912,
                "swap": 536870912,
                "kernel": -1,
                "kernelTCP": -1,
                "swappiness": 0,
                "disableOOMKiller": false
            },
            "cpu": {
                "shares": 1024,
                "quota": 1000000,
                "period": 500000,
                "realtimeRuntime": 950000,
                "realtimePeriod": 1000000,
                "cpus": "2-3",
                "idle": 1,
                "mems": "0-7"
            },
            "devices": [
                {
                    "allow": false,
                    "access": "rwm"
                },
                {
                    "allow": true,
                    "type": "c",
                    "major": 10,
                    "minor": 229,
                    "access": "rw"
                },
                {
                    "allow": true,
                    "type": "b",
                    "major": 8,
                    "minor": 0,
                    "access": "r"
                }
            ],
            "blockIO": {
                "weight": 10,
                "leafWeight": 10,
                "weightDevice": [
                    {
                        "major": 8,
                        "minor": 0,
                        "weight": 500,
                        "leafWeight": 300
                    },
                    {
                        "major": 8,
                        "minor": 16,
                        "weight": 500
                    }
                ],
                "throttleReadBpsDevice": [
                    {
                        "major": 8,
                        "minor": 0,
                        "rate": 600
                    }
                ],
                "throttleWriteIOPSDevice": [
                    {
                        "major": 8,
                        "minor": 16,
                        "rate": 300
                    }
                ]
            }
        },
        "rootfsPropagation": "slave",
        "seccomp": {
            "defaultAction": "SCMP_ACT_ALLOW",
            "architectures": [
                "SCMP_ARCH_X86",
                "SCMP_ARCH_X32"
            ],
            "syscalls": [
                {
                    "names": [
                        "getcwd",
                        "chmod"
                    ],
                    "action": "SCMP_ACT_ERRNO"
                }
            ]
        },
        "namespaces": [
            {
                "type": "pid"
            },
            {
                "type": "network"
            },
            {
                "type": "ipc"
            },
            {
                "type": "uts"
            },
            {
                "type": "mount"
            },
            {
                "type": "user"
            },
            {
                "type": "cgroup"
            }
        ],
        "maskedPaths": [
            "/proc/kcore",
            "/proc/latency_stats",
            "/proc/timer_stats",
            "/proc/sched_debug"
        ],
        "readonlyPaths": [
            "/proc/asound",
            "/proc/bus",
            "/proc/fs",
            "/proc/irq",
            "/proc/sys",
            "/proc/sysrq-trigger"
        ],
        "mountLabel": "system_u:object_r:svirt_sandbox_file_t:s0:c715,c811"
    },
    "annotations": {
        "com.example.key1": "value1",
        "com.example.key2": "value2"
    }
}
```


[apparmor]: https://wiki.ubuntu.com/AppArmor
[cgroup-v1-memory_2]: https://www.kernel.org/doc/Documentation/cgroup-v1/memory.txt
[selinux]:http://selinuxproject.org/page/Main_Page
[no-new-privs]: https://www.kernel.org/doc/Documentation/prctl/no_new_privs.txt
[proc_2]: https://www.kernel.org/doc/Documentation/filesystems/proc.txt
[umask.2]: http://pubs.opengroup.org/onlinepubs/009695399/functions/umask.html
[semver-v2.0.0]: http://semver.org/spec/v2.0.0.html
[ieee-1003.1-2008-xbd-c8.1]: http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap08.html#tag_08_01
[ieee-1003.1-2008-functions-exec]: http://pubs.opengroup.org/onlinepubs/9699919799/functions/exec.html
[naming-a-volume]: https://aka.ms/nb3hqb

[capabilities.7]: http://man7.org/linux/man-pages/man7/capabilities.7.html
[mount.2]: http://man7.org/linux/man-pages/man2/mount.2.html
[mount.8]: http://man7.org/linux/man-pages/man8/mount.8.html
[mount.8-filesystem-independent]: http://man7.org/linux/man-pages/man8/mount.8.html#FILESYSTEM-INDEPENDENT_MOUNT_OPTIONS
[mount.8-filesystem-specific]: http://man7.org/linux/man-pages/man8/mount.8.html#FILESYSTEM-SPECIFIC_MOUNT_OPTIONS
[getrlimit.2]: http://man7.org/linux/man-pages/man2/getrlimit.2.html
[getrlimit.3]: http://pubs.opengroup.org/onlinepubs/9699919799/functions/getrlimit.html
[stdin.3]: http://man7.org/linux/man-pages/man3/stdin.3.html
[uts-namespace.7]: http://man7.org/linux/man-pages/man7/namespaces.7.html
[zonecfg.1m]: http://docs.oracle.com/cd/E86824_01/html/E54764/zonecfg-1m.html

---

通过 www.DeepL.com/Translator（免费版）翻译, 然后手工校对。