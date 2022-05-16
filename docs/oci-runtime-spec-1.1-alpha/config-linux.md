# <a name="linuxContainerConfiguration" />Linux 容器配置

本文描述了 [容器配置](config.md) 的 [Linux 专用部分](config.md#platform-specific-configuration) 的模式。
Linux 容器规范使用各种内核特性，如 namespaces, cgroups, capabilities, LSM 以及 filesystem jails 来实现该规范。

## <a name="configLinuxDefaultFilesystems" />Default Filesystems

Linux ABI 包括系统调用和一些特殊的文件路径。
期待 Linux 环境的应用程序很可能期望这些文件路径被正确设置。

以下文件系统应该在每个容器的文件系统中可用。

| 路径     | 类型       |
| -------- | ---------- |
| /proc    | [proc][]   |
| /sys     | [sysfs][]  |
| /dev/pts | [devpts][] |
| /dev/shm | [tmpfs][]  |

## <a name="configLinuxNamespaces" />Namespaces

命名空间将全局系统资源包裹在一个抽象中，使命名空间中的进程看起来有自己的全局资源的孤立实例。
对全局资源的改变对作为命名空间成员的其他进程是可见的，但对其他进程是不可见的。
更多信息，请参阅 [namespaces(7)][namespaces.7_2] man page。

命名空间被指定为`namespaces`根字段内的条目数组。
可以指定以下参数来设置命名空间。

- **`type`** *(string, REQUIRED)* - 命名空间类型。应该支持以下命名空间类型。
    - **`pid`** 容器内的进程将只能看到同一容器内或同一 pid 命名空间内的其他进程。
    - **`network`** 容器将有自己的网络栈。
    - **`mount`** 容器将有一个隔离的挂载表。
    - **`ipc`** 容器内的进程只能通过系统级IPC与同一容器内的其他进程通信。
    - **`uts`** 容器将能够拥有自己的主机名和域名。
    - **`user`** 容器将能够把主机上的用户和组ID重新映射到容器内的本地用户和组。
    - **`cgroup`** 容器将有一个孤立的cgroup层次结构的视图。
- **`path`** *(string, OPTIONAL)* - 命名空间文件。
    这个值必须是 [运行时命名空间](glossary.md#runtime-namespace) 中的绝对路径。
    运行时必须将容器进程放在与该 "路径" 相关的命名空间中。
    如果 `path` 没有与 `type` 类型的命名空间相关联，则运行时必须[产生一个错误](runtime.md#errors)。

    如果没有指定 `path`，运行时必须创建一个新的 `type` 类型的[容器命名空间](glossary.md#container-namespace)。

如果在 `namespaces` 数组中没有指定命名空间类型，那么容器必须继承该类型的 [运行时命名空间](glossary.md#runtime-namespace)。
如果 `namespaces` 字段包含具有相同 `类型` 的重复命名空间，运行时必须[产生一个错误](runtime.md#errors)。

### 例子

```json
"namespaces": [
    {
        "type": "pid",
        "path": "/proc/1234/ns/pid"
    },
    {
        "type": "network",
        "path": "/var/run/netns/neta"
    },
    {
        "type": "mount"
    },
    {
        "type": "ipc"
    },
    {
        "type": "uts"
    },
    {
        "type": "user"
    },
    {
        "type": "cgroup"
    }
]
```

## <a name="configLinuxUserNamespaceMappings" />User namespace mappings

**`uidMappings`** (object[]，OPTIONAL) 描述了 从主机到容器 的用户命名空间 uid 映射。
**`gidMappings`** (object[]，OPTIONAL) 描述了 从主机到容器 的用户名称空间 gid 映射。

每个条目都有以下结构。

- **`containerID`** *(uint32, REQUIRED)* - 是容器中的起始uid/gid。
- **`hostID`** *(uint32, REQUIRED)* - 是要映射到*containerID*的主机上的起始uid/gid。
- **`size`** *(uint32, REQUIRED)* - 是要映射的ID的数量。

运行时不应该为了实现映射而修改引用的文件系统的所有权。
请注意，映射项的数量可能会受到 [kernel][user-namespaces] 的限制。

### 例子

```json
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
]
```

## <a name="configLinuxDevices" />Devices

**`devices`** (对象数组，可选)列出容器中必须有的设备。
运行时可以以它喜欢的方式提供这些设备（使用 [`mknod`][mknod.2]，通过从运行时 mount 命名空间绑定安装，使用符号链接，等等）。

每个条目都有以下结构。

- **`type`** *(string, REQUIRED)* - 设备的类型：`c`, `b`, `u`或`p`。
    更多信息见[mknod(1)][mknod.1]。
- **`path`** *(string, REQUIRED)* - 容器内设备的完整路径。
    如果在 `path` 处已经存在一个[file][]，但与请求的设备不匹配，运行时必须产生一个错误。
- **`major, minor`** *(int64, REQUIRED unless `type` is `p`)* - 设备的 [major, minor numbers][devices]。
- **`fileMode`** *(uint32, OPTIONAL)* - 设备的文件模式。
    你也可以[用cgroups](#configLinuxDeviceAllowedlist)控制对设备的访问。
- **`uid`** *(uint32, OPTIONAL)* - 设备所有者在[容器命名空间](glossary.md#container-namespace)的id。
- **`gid`** *(uint32, OPTIONAL)* - [容器命名空间](glossary.md#container-namespace) 中设备组的id。

相同的 `type`, `major` 和 `minor` 不应被用于多个设备。

### 例子

```json
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
]
```

### <a name="configLinuxDefaultDevices" />Default Devices

除了用此设置配置的任何设备外，运行时还必须提供。

* [`/dev/null`][null.4]。
* [`/dev/zero`][zero.4]。
* [`/dev/full`][full.4]。
* [`/dev/random`][random.4]
* [`/dev/urandom`][random.4] 。
* [`/dev/tty`][tty.4]
* `/dev/console` 是在配置中启用了 [`terminal`](config.md#process) 后设置的，通过绑定挂载伪终端 pty 到 `/dev/console` 。
* [`/dev/ptmx`][pts.4].
  一个 [容器的 `/dev/pts/ptmx` 的绑定挂载或符号链接][devpts]。

## <a name="configLinuxControlGroups" />Control groups

也被称为 cgroups，它们被用来限制容器的资源使用和处理设备访问。
cgroups 提供控制（通过控制器）来限制容器的 cpu、内存、IO、pids、网络和 RDMA 资源。
更多信息，请参见 [kernel cgroups documentation][cgroup-v1]。

在一个特定的[容器操作](runtime.md#operation)过程中，例如 [create](runtime.md#create)、[start](runtime.md#start)、或
[exec](runtime.md#exec)，运行时可能会检查容器 cgroup 是否适合使用。
如果检查失败，则必须 [生成一个错误](runtime.md#errors)。
例如，一个冻结的 cgroup 或（对于[创建](runtime.md#create)操作）一个非空的cgroup。
这样做的原因是，接受这样的配置
可能会导致用户可能无法预期或理解的容器操作结果。
的结果，例如对一个容器的操作会无意中影响到其他的
容器。

### <a name="configLinuxCgroupsPath" />Cgroups Path

**`cgroupsPath`** (string, OPTIONAL) 通往 cgroups 的路径。
它可以用来控制容器的 cgroups hierarchy，也可以用来在现有的容器中运行一个新进程。

cgroupsPath 的值必须是一个绝对路径或一个相对路径。

* 如果是绝对路径（以`/`开头），运行时必须认为该路径是相对于 cgroups 装载点的。
* 如果是相对路径（不以`/`开头），运行时可以将路径解释为相对于运行时确定的 cgroups hierarchy，也可以用来在现有的容器中运行一个新进程。 中的一个位置。

如果指定了该值，运行时必须在给定相同的 `cgroupsPath` 值的情况下一致地附加到 cgroups hierarchy，也可以用来在现有的容器中运行一个新进程。中的同一位置。
如果没有指定该值，运行时可以定义默认的cgroups路径。
运行时可能会认为某些`cgroupsPath'值是无效的，如果是这种情况，必须产生一个错误。

如果指定了该值，则在给定相同的 `cgroupsPath` 值的情况下，运行时必须始终附加到 cgroups hierarchy 中的相同位置。如果未指定该值，运行时可能会定义默认的 cgroups 路径。运行时可能会认为某些 `cgroupsPath` 值无效，如果是这种情况，则必须生成错误。

本规范的实现可以选择以任何方式来命名cgroups。
本规范不包括 cgroups 的命名模式。
由于 [cgroup v2文档][cgroup-v2] 中讨论的原因，本规范不支持每个控制器的路径。
如果 cgroup 不存在，就会被创建。

你可以通过 Linux 配置中的 `resources` 字段来配置容器的 cgroups。
不要指定`resources`，除非必须更新限制。
例如，要在现有的容器中运行一个新的进程而不更新限制，就不需要指定 `resources`。

运行时可能会将容器进程附加到额外的 cgroup 控制器上，而不是满足 `resources` 设置所需的那些控制器。

### Cgroup 所有权

根据下面的规则，运行时可以将容器的 cgroup 的所有者改变（或导致改变）为与[容器命名空间](glossary.md#container-namespace)中 `process.user.uid` 的值相对应的主机 uid；也就是说，将执行容器进程的用户。

当 cgroups v1 正在使用时，运行时不应该改变容器 cgroup 的所有权。在 cgroups v1 中，cgroup delegation 是不安全的。

运行时不应该改变容器 cgroup 的所有权，除非它还将为该容器创建一个新的 cgroup 命名空间。
一个典型的情况是，当 `linux.namespaces` 数组中包含一个对象，其 `type` 等于 `cgroup`，`path` 未设置。

运行时应该改变cgroup的所有权，当且仅当 cgroup 文件系统将被挂载为 read/write；也就是说，当
配置的 `mounts` 数组包含一个对象，其中：

- `source` 字段等于 `cgroup`
- `destination` 字段等于 `/sys/fs/cgroup`
- `options` 字段不包含值 `"ro"`

如果配置中没有指定这样的挂载，运行时不应改变 cgroup 的所有权。

改变了 cgroup 所有权的运行时，应该只改变容器的 cgroup 目录和该目录中的文件的所有权。
目录中的文件的所有权，这些文件列在 `/sys/kernel/cgroup/delegate` 中。 参见
`cgroups(7)` 了解关于这个文件的细节。注意，并非所有列在 `/sys/kernel/cgroup/delegate`
中的文件都一定存在于每个 cgroup 中。在这种情况下，运行时不能失败，并且应该改变 cgroup 中确实存在的所列文件的所有权。

如果 `/sys/kernel/cgroup/delegate` 文件不存在，则运行时必须退回到使用以下文件列表。

```
cgroup.procs
cgroup.subtree_control
cgroup.threads
```

运行时不应该改变任何其他文件的所有权。
改变其他文件可能会使容器提高自己的
资源限制或执行其他不需要的行为。

### 例子

```json
"cgroupsPath": "/myRuntime/myContainer",
"resources": {
    "memory": {
        "limit": 100000,
        "reservation": 200000
    },
    "devices": [
        {
            "allow": false,
            "access": "rwm"
        }
    ]
}
```

### <a name="configLinuxDeviceAllowedlist" />允许的设备列表

**`devices`** (object[]，OPTIONAL) 配置 [允许的设备列表][cgroup-v1-devices]。
运行时必须按照列出的顺序应用条目。

每个条目有以下结构：

- **`allow`** *(boolean, REQUIRED)* - 该条目是否被允许或拒绝。
- **`type`** *(string, OPTIONAL)* - 设备的类型：`a` (所有)，`c` (char)，或 `b` (block)。
    未设置的值意味着 "all"，映射到 `a`。
- **`major, minor`** *(int64, OPTIONAL)* - 设备的[major, minor 数字][devices]。
    未设置的值意味着 "all"，映射到[文件系统 API 中的`*`][cgroup-v1-devices]。
- **`access`** *(string, OPTIONAL)* - 设备的cgroup权限。
    由`r`（读）、`w`（写）和`m`（mknod）组成。

#### 例子

```json
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
]
```

### <a name="configLinuxMemory" />Memory

**`memory`** (object, OPTIONAL) 代表 cgroup 子系统 `memory`，它用于设置容器的内存使用限制。
更多信息，请参见内核 cgroup 文档中的 [memory][cgroup-v1-memory]。

内存的值以字节为单位指定限制，或以 `-1` 表示无限的内存。

- **`limit`** *(int64, OPTIONAL)* - 设置内存使用限制
- **`reservation`** *(int64, OPTIONAL)* - 设置内存使用的软限制。
- **`swap`** *(int64, OPTIONAL)* - 设置 内存+Swap 的使用限制。
- **`kernel`** *(int64, OPTIONAL, NOT RECOMMENDED)* - 设置内核内存的硬限制。
- **`kernelTCP`** *(int64, OPTIONAL, NOT RECOMMENDED)* - 设置内核TCP缓冲区内存的硬限制。

以下属性没有指定内存限制，但被 `memory` 控制器所覆盖:

- **`swappiness`** *(uint64, OPTIONAL)* - 设置vmscan的swappiness参数(见sysctl的vm.swappiness)
    值从0到100。更高的值意味着更多的互换性。
- **`disableOOMKiller`** *(bool, OPTIONAL)* - 启用或禁用 OOM killer。
    如果启用(`false`)，试图消耗超过其允许的内存的任务将立即被 OOM killer 杀死。
    OOM killer 在每个使用 `memory` 子系统的 cgroup 中默认是启用的。
    要禁用它，请指定为 `true`。
- **`useHierarchy`** *(bool, OPTIONAL)* - 启用或禁用分层内存审计。
    如果启用(`true`)，子 cgroups 将共享这个 cgroup 的内存限制。

#### 例子

```json
"memory": {
    "limit": 536870912,
    "reservation": 536870912,
    "swap": 536870912,
    "kernel": -1,
    "kernelTCP": -1,
    "swappiness": 0,
    "disableOOMKiller": false
}
```

### <a name="configLinuxCPU" />CPU

**`cpu`** (object, OPTIONAL) 代表 cgroup 子系统 `cpu` 和 `cpusets`。
更多信息，请参阅内核 cgroups 文档中关于 [cpusets][cgroup-v1-cpusets] 的内容。

可以指定以下参数来设置控制器。

- **`shares`** *(uint64, OPTIONAL)* - 指定在一个 cgroup 中 tasks 可用的 CPU 时间的相对份额
- **`quota`** *(int64, OPTIONAL)* - 指定 cgroup 中的所有 tasks 在一个 `period` 内可以运行的总时间（以微秒为单位）（下面定义了 `period`）
- **`period`** *(uint64, OPTIONAL)* - 指定以微秒为单位的时间段，一个 cgroup 对 CPU 资源的访问应该如何定期重新分配（仅 CFS 调度器）。
- **`realtimeRuntime`** *(int64, OPTIONAL)* - 以微秒为单位指定 cgroup 中的任务访问 CPU 资源的最长连续时间。
- **`realtimePeriod`** *(uint64, OPTIONAL)* - 与 **`period`** 相同，但只适用于实时调度器
- **`cpus`** *(string, OPTIONAL)* - 容器将要运行的CPU的列表
- **`mems`** *(string, OPTIONAL)* - 容器将运行的内存节点列表
- **`idle`** *(int64, OPTIONAL)* - cgroups被配置为最小权重，0: 默认行为，1: SCHED_IDLE。

#### 例子

```json
"cpu": {
    "shares": 1024,
    "quota": 1000000,
    "period": 500000,
    "realtimeRuntime": 950000,
    "realtimePeriod": 1000000,
    "cpus": "2-3",
    "mems": "0-7",
    "idle": 0
}
```

### <a name="configLinuxBlockIO" />Block IO

**`blockIO`** (object, OPTIONAL) 表示 cgroup 子系统 `blkio`，它实现了块状IO控制器。
更多信息，请参见内核 cgroup 文档中关于[blkio][cgroup-v1-blkio]。

可以指定以下参数来设置控制器。

- **`weight`** *(uint16, OPTIONAL)* - 指定每个 cgroup 的 weight。这是该组在所有设备上的默认权重，除非被每个设备的规则覆盖。
- **`leafWeight`** *(uint16, OPTIONAL)* - 等同于 `weight`，用于决定给定 cgroup 中的任务在与c组的子c组竞争时有多少权重。
- **`weightDevice`** *(array of objects, OPTIONAL)* - 每个设备的带宽权重数组。
    每个条目都有以下结构。
    - **`major, minor`** *(int64, REQUIRED)* - 设备的 major, minor 数字。
        更多信息请参见 [mknod(1)][mknod.1] man page。
    - **`weight`** *(uint16, OPTIONAL)* - 设备的带宽权重。
    - **`leafWeight`** *(uint16, OPTIONAL)* - 设备与 cgroup 的子 cgroup 竞争时的带宽权重，仅适用于 CFQ 调度器

    你必须在给定的条目中至少指定 `weight` 或 `leafWeight` 中的一个，并且可以同时指定。

- **`throttleReadBpsDevice`**, **`throttleWriteBpsDevice`** *(array of objects, OPTIONAL)* - 每个设备的带宽速率限制数组。
    每个条目都有以下结构。
    - **`major, minor`** *(int64, REQUIRED)* - 设备的 major, minor 数字。
        更多信息请参见 [mknod(1)][mknod.1] man page。
    - **`rate`** *(uint64, REQUIRED)* - 设备的带宽速率限制，单位为 字节/秒

- **`throttleReadIOPSDevice`**, **`throttleWriteIOPSDevice`** *(array of objects, OPTIONAL)* - 每个设备的IO速率限制数组。
    每个条目都有以下结构。
    - **`major, minor`** *(int64, REQUIRED)* - 设备的 major, minor 数字。
        更多信息，请参阅 [mknod(1)][mknod.1] man page。
    - **`rate`** *(uint64, REQUIRED)* - 设备的IO速率限制

#### 示例

```json
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
```

### <a name="configLinuxHugePageLimits" />Huge page limits

**`hugepageLimits`** (object[]，OPTIONAL) 代表 `hugetlb` 控制器，它允许限制每个控制组的
HugeTLB 的使用，并在页面故障时执行控制器的限制。
更多信息，请参阅内核 cgroups 关于 [HugeTLB][cgroup-v1-hugetlb] 的文档。

每个条目都有以下结构。

- **`pageSize`** *(string, REQUIRED)* - hugepage size.
    这个值的格式是 `<size><unit-prefix>B` (64KB, 2MB, 1GB)，并且必须与在 `/sys/fs/cgroup/hugetlb/hugetlb.<hugepagesize>.limit_in_bytes` 中找到的相应控制文件的 `<hugepagesize>`相匹配。
    `<unit-prefix>` 的值旨在使用 1024 进制进行解析（"1KB"=1024，"1MB"=1048576，等等）。
- **limit`** *(uint64, REQUIRED)* - *hugepagesize* HugeTLB使用的字节数限制

#### 例子

```json
"hugepageLimits": [
    {
        "pageSize": "2MB",
        "limit": 209715200
    },
    {
        "pageSize": "64KB",
        "limit": 1000000
    }
]
```

### <a name="configLinuxNetwork" />Network

**`network`** (object, OPTIONAL) 代表 cgroup 子系统 `net_cls` 和 `net_prio`。
更多信息，请参阅内核cgroup文档中关于[net_cls cgroup][cgroup-v1-net-cls]和[net_prio cgroup][cgroup-v1-net-prio]的内容。

可以指定以下参数来设置控制器。

- **`classID`** *(uint32, OPTIONAL)* - 是网络类标识符，cgroup 的网络数据包将被标记。
- **`priorities`** *(object[], OPTIONAL)* - 一个对象列表，指定分配给来自组内进程，在不同接口上离开系统的流量的优先级。
    可以为每个条目指定以下参数。
    - **`name`** *(string, REQUIRED)* - [runtime network namespace](glossary.md#runtime-namespace) 中的接口名称。
    - **`priority`** *(uint32, REQUIRED)* - 应用于该接口的优先级

#### 示例

```json
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
}
```

### <a name="configLinuxPIDS" />PIDs

**`pids`** (object, OPTIONAL) 表示 cgroup 子系统的 `pids`。
更多信息，请参阅内核 cgroup 文档中关于 [pids][cgroup-v1-pids] 的内容。

可以指定以下参数来设置控制器。

- **`limit`** *(int64, REQUIRED)* - 指定 cgroup 中 tasks 的最大数量

#### 例子

```json
"pids": {
    "limit": 32771
}
```

### <a name="configLinuxRDMA" />RDMA

**`rdma`** (object, OPTIONAL) 代表 cgroup 子系统 `rdma`。
更多信息，请看内核 cgroup 文档关于 [rdma][cgroup-v1-rdma]。

要限制的设备的名称是各个条目的 key。条目的 value 是具有以下属性的对象。

- **`hcaHandles`** *(uint32, OPTIONAL)* - 指定 cgroup 中 hca_handles 的最大数量
- **`hcaObjects`** *(uint32, OPTIONAL)* - 指定 cgroup 中 hca_objects 的最大数量。

你必须在一个给定的条目中至少指定一个 `hcaHandles` 或 `hcaObjects`，也可以同时指定。

#### 示例

```json
"rdma": {
    "mlx5_1": {
        "hcaHandles": 3,
        "hcaObjects": 10000
    },
    "mlx4_0": {
        "hcaObjects": 1000
    },
    "rxe3": {
        "hcaObjects": 10000
    }
}
```

## <a name="configLinuxUnified" />Unified

**`unified`** (object, OPTIONAL) 允许为容器设置和修改 cgroup v2 参数。

Map 中的每个 key 指的是 cgroup unified hierarchy 中的一个文件。

OCI 运行时必须确保为 cgroup 启用所需的 cgroup 控制器。

运行时未知的配置必须仍然被写入相关文件。

当配置指的是一个不存在或无法启用的 cgroup 控制器时，运行时必须产生一个错误。

### 例子

```json
"unified": {
    "io.max": "259:0 rbps=2097152 wiops=120\n253:0 rbps=2097152 wiops=120",
    "hugetlb.1GB.max": "1073741824"
}
```

如果控制器在 cgroup v2 层次结构上被启用，但配置是为 cgroup v1 对应的控制器提供的，则运行时可能会尝试转换。

如果转换不成功，运行时必须产生一个错误。
## <a name="configLinuxIntelRdt" />IntelRdt

**`intelRdt`** (object, OPTIONAL) represents the [Intel Resource Director Technology][intel-rdt-cat-kernel-interface].
If `intelRdt` is set, the runtime MUST write the container process ID to the `tasks` file in a proper sub-directory in a mounted `resctrl` pseudo-filesystem. That sub-directory name is specified by `closID` parameter.
If no mounted `resctrl` pseudo-filesystem is available in the [runtime mount namespace](glossary.md#runtime-namespace), the runtime MUST [generate an error](runtime.md#errors).

If `intelRdt` is not set, the runtime MUST NOT manipulate any `resctrl` pseudo-filesystems.

The following parameters can be specified for the container:

* **`closID`** *(string, OPTIONAL)* - specifies the identity for RDT Class of Service (CLOS).

* **`l3CacheSchema`** *(string, OPTIONAL)* - specifies the schema for L3 cache id and capacity bitmask (CBM).
    The value SHOULD start with `L3:` and SHOULD NOT contain newlines.
* **`memBwSchema`** *(string, OPTIONAL)* - specifies the schema of memory bandwidth per L3 cache id.
    The value MUST start with `MB:` and MUST NOT contain newlines.

The following rules on parameters MUST be applied:

* If both `l3CacheSchema` and `memBwSchema` are set, runtimes MUST write the combined value to the `schemata` file in that sub-directory discussed in `closID`.

* If `l3CacheSchema` contains a line beginning with `MB:`, the value written to `schemata` file MUST be the non-`MB:` line(s) from `l3CacheSchema` and the line from `memBWSchema`.

* If either `l3CacheSchema` or `memBwSchema` is set, runtimes MUST write the value to the `schemata` file in the that sub-directory discussed in `closID`.

* If neither `l3CacheSchema` nor `memBwSchema` is set, runtimes MUST NOT write to `schemata` files in any `resctrl` pseudo-filesystems.

* If `closID` is not set, runtimes MUST use the container ID from [`start`](runtime.md#start) and create the `<container-id>` directory.

* If `closID` is set, `l3CacheSchema` and/or `memBwSchema` is set
  * if `closID` directory in a mounted `resctrl` pseudo-filesystem doesn't exist, the runtimes MUST create it.
  * if `closID` directory in a mounted `resctrl` pseudo-filesystem exists, runtimes MUST compare `l3CacheSchema` and/or `memBwSchema` value with `schemata` file, and [generate an error](runtime.md#errors) if doesn't match.

* If `closID` is set, and neither of `l3CacheSchema` and `memBwSchema` are set, runtime MUST check if corresponding pre-configured directory `closID` is present in mounted `resctrl`. If such pre-configured directory `closID` exists, runtime MUST assign container to this `closID` and [generate an error](runtime.md#errors) if directory does not exist.

* **`enableCMT`** *(boolean, OPTIONAL)* - specifies if Intel RDT CMT should be enabled:
    * CMT (Cache Monitoring Technology) supports monitoring of the last-level cache (LLC) occupancy
      for the container.

* **`enableMBM`** *(boolean, OPTIONAL)* - specifies if Intel RDT MBM should be enabled:
    * MBM (Memory Bandwidth Monitoring) supports monitoring of total and local memory bandwidth
      for the container.

### Example

Consider a two-socket machine with two L3 caches where the default CBM is 0x7ff and the max CBM length is 11 bits,
and minimum memory bandwidth of 10% with a memory bandwidth granularity of 10%.

Tasks inside the container only have access to the "upper" 7/11 of L3 cache on socket 0 and the "lower" 5/11 L3 cache on socket 1,
and may use a maximum memory bandwidth of 20% on socket 0 and 70% on socket 1.

```json
"linux": {
    "intelRdt": {
        "closID": "guaranteed_group",
        "l3CacheSchema": "L3:0=7f0;1=1f",
        "memBwSchema": "MB:0=20;1=70"
    }
}
```


## <a name="configLinuxSysctl" />Sysctl

**`sysctl`** (object, OPTIONAL) 允许在运行时为容器修改内核参数。
更多信息，请参阅 [sysctl(8)][sysctl.8] man page。

### 例子

```json
"sysctl": {
    "net.ipv4.ip_forward": "1",
    "net.core.somaxconn": "256"
}
```

## <a name="configLinuxSeccomp" />Seccomp

Seccomp 在 Linux 内核中提供应用沙箱机制。
Seccomp 的配置允许人们配置对匹配的系统调用所采取的行动，此外还允许对作为参数传递给系统调用的值进行匹配。
关于 Seccomp 的更多信息，请参见 [Seccomp][seccomp] 内核文档。
动作、架构和操作符是与 [libseccomp][] 的 seccomp.h 中的定义相匹配的字符串，并被翻译成相应的值。

**`seccomp`** (object, OPTIONAL)

可以指定以下参数来设置seccomp。

- **`defaultAction`** *(string, REQUIRED)* - seccomp 的默认动作。允许的值与 `syscalls[].action` 相同。
- **`defaultErrnoRet`** *(uint, OPTIONAL)* - 使用的 Errno 返回代码。
    一些动作如 `SCMP_ACT_ERRNO` 和 `SCMP_ACT_TRACE` 允许指定返回的 Errno 代码。
    当动作不支持 errno 时，运行时必须打印错误并失败。
    如果没有指定，那么其默认值为 `EPERM` 。
- **`architectures`** *(array of strings, OPTIONAL)* - 用于系统调用的架构。
    libseccomp v2.5.0 的有效常量列表如下。

    * `SCMP_ARCH_X86`
    * `SCMP_ARCH_X86_64`
    * `SCMP_ARCH_X32`
    * `SCMP_ARCH_ARM`
    * `SCMP_ARCH_AARCH64`
    * `SCMP_ARCH_MIPS`
    * `SCMP_ARCH_MIPS64`
    * `SCMP_ARCH_MIPS64N32`
    * `SCMP_ARCH_MIPSEL`
    * `SCMP_ARCH_MIPSEL64`
    * `SCMP_ARCH_MIPSEL64N32`
    * `SCMP_ARCH_PPC`
    * `SCMP_ARCH_PPC64`
    * `SCMP_ARCH_PPC64LE`
    * `SCMP_ARCH_S390`
    * `SCMP_ARCH_S390X`
    * `SCMP_ARCH_PARISC`
    * `SCMP_ARCH_PARISC64`
    * `SCMP_ARCH_RISCV64`

- **`flags`** *(字符串数组，可选的)* - 与 seccomp(2) 一起使用的标志列表。

    一个有效的常量列表如下所示。

    * `SECCOMP_FILTER_FLAG_TSYNC`
    * `SECCOMP_FILTER_FLAG_LOG`
    * `SECCOMP_FILTER_FLAG_SPEC_ALLOW`

- **`listenerPath`** *(string, OPTIONAL)* - 指定 UNIX 域套接字的路径，当使用 `SCMP_ACT_NOTIFY` 动作时，运行时将通过它发送[容器进程状态](#containerprocessstate) 数据结构。
    这个套接字必须使用 `AF_UNIX` 域和 `SOCK_STREAM` 类型。
    运行时必须为每个连接准确发送一个[容器进程状态](#containerprocessstate)。
    该连接不得重复使用，在发送一个 seccomp 状态后必须关闭。
    如果向该套接字发送失败，运行时必须[产生一个错误](runtime.md#errors)。
    如果没有使用 `SCMP_ACT_NOTIFY` 动作，这个值会被忽略。

    运行时使用 `SCM_RIGHTS` 发送以下文件描述符，并在[容器进程状态](#containerprocessstate)的 `fds`数组中设置其名称。

    - **`seccompFd`** (string, REQUIRED) 是 seccomp syscall 返回的 seccomp 文件描述符。

- **`listenerMetadata`** *(string, OPTIONAL)* - 指定一个不透明的数据传递给seccomp代理。
    这个字符串将作为[容器进程状态](#containerprocessstate)中的`metadata`字段发送。
    如果没有设置`listenerPath'，这个字段一定不能设置。

- **`syscalls`** *(对象数组，OPTIONAL)* - 匹配 seccomp 中的 syscall。
    虽然这个属性是 OPTIONAL，但如果没有 `syscalls` 条目，`defaultAction` 的某些值就没有用。
    例如，如果`defaultAction`是`SCMP_ACT_KILL`，而`syscalls`为空或未设置，内核将在容器进程的第一个系统调用中杀死它。
    每个条目都有以下结构。

    - **`names`** *（字符串数组，必须）* - 系统调用的名称。
        `names`必须至少包含一个条目。
    - **`action`** *(string, REQUIRED)* - seccomp规则的动作。
        libseccomp v2.5.0的有效常量列表如下。

        * `SCMP_ACT_KILL`
        * `SCMP_ACT_KILL_PROCESS`
        * `SCMP_ACT_KILL_THREAD`
        * `SCMP_ACT_TRAP`
        * `SCMP_ACT_ERRNO`
        * `SCMP_ACT_TRACE`
        * `SCMP_ACT_ALLOW`
        * `SCMP_ACT_LOG`
        * `SCMP_ACT_NOTIFY`

    - **`errnoRet`** *(uint, OPTIONAL)* - 要使用的errno返回代码。
        一些动作如 `SCMP_ACT_ERRNO` 和 `SCMP_ACT_TRACE` 允许指定返回的errno代码。
        当动作不支持errno时，运行时必须打印错误并失败。
        如果没有指定，其默认值为`EPERM`。

    - **`args`** *(object[]，OPTIONAL)* - seccomp 中的特定系统调用。
        每个条目都有以下结构。

        - **`index`** *(uint, REQUIRED)* - seccomp 中系统调用参数的索引。
        - **`value`** *(uint64, REQUIRED)* - seccomp 中 syscall 参数的值。
        - **`valueTwo`** *(uint64, OPTIONAL)* - seccomp 中 syscall 参数的值。
        - **`op`** *(string, REQUIRED)* - seccomp 中系统调用参数的操作符。
            libseccomp v2.3.2 的有效常量列表如下。

            * `SCMP_CMP_NE`
            * `SCMP_CMP_LT`
            * `SCMP_CMP_LE`
            * `SCMP_CMP_EQ`
            * `SCMP_CMP_GE`
            * `SCMP_CMP_GT`
            * `SCMP_CMP_MASKED_EQ`

### 示例

```json
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
}
```

### <a name="containerprocessstate" />容器进程状态

容器进程状态是一个通过 UNIX 套接字传递的数据结构。
容器运行时必须通过 UNIX 套接字将容器进程状态作为常规的有效载荷以 JSON 的形式序列化，文件描述符必须使用 `SCM_RIGHTS` 来发送。
容器运行时可以使用几个 `sendmsg(2)` 调用来发送上述数据。
如果使用一个以上的 `sendmsg(2)`，文件描述符必须只在第一次调用中发送。

容器进程状态包括以下属性。

- **`ociVersion`** (string, REQUIRED) 是容器进程状态符合的Open Container Initiative Runtime Specification的版本。
- **`fds`** (array, OPTIONAL) 是一个字符串数组，包含所传递的文件描述符的名称。
    这个数组中的名称索引与`SCM_RIGHTS`数组中的文件描述符的索引相对应。
- **`pid`** (int, REQUIRED) 是容器进程的ID，由运行时看到。
- **`metadata`** (string, OPTIONAL) 不透明的元数据。
- **`state`** （[state](runtime.md#state), REQUIRED）是容器的状态。

在 `SCM_RIGHTS` 阵列中发送单个 `seccompFd` 文件描述符的例子：

```json
{
    "ociVersion": "0.2.0",
    "fds": [
        "seccompFd"
    ],
    "pid": 4422,
    "metadata": "MKNOD=/dev/null,/dev/net/tun;BPF_MAP_TYPES=hash,array",
    "state": {
        "ociVersion": "0.2.0",
        "id": "oci-container1",
        "status": "creating",
        "pid": 4422,
        "bundle": "/containers/redis",
        "annotations": {
            "myKey": "myValue"
        }
    }
}
```

## <a name="configLinuxRootfsMountPropagation" />Rootfs 挂载传播

**`rootfsPropagation`** (string, OPTIONAL) 设置 rootfs 的挂载传播方式。
它的值是 `shared`, `slave`, `private` 或 `unbindable`。
值得注意的是，一个对等组被定义为一组相互传播事件的 VFS 挂载。
嵌套容器被定义为在一个现有的容器内启动的容器。

- **`shared`**：rootfs挂载属于一个新的对等组。
    这意味着更多的挂载（例如嵌套容器）也将属于该对等组，并将事件传播到rootfs。
    注意这并不意味着它是与主机共享的。
- **`slave`**: rootfs挂载接收来自主机的传播事件（例如，如果有东西挂载在主机上，它也会出现在容器中），但不是反过来的。
- **`private`**: rootfs挂载不会接收来自主机的挂载传播事件，嵌套容器中的进一步挂载将与主机和rootfs隔离（即使嵌套容器的`rootfsPropagation`选项是共享的）。
- **`unbindable`**: rootfs 挂载是一个 private 挂载，不能被绑定挂载。

内核文档中的 [Shared Subtrees][sharedsubtree] 文章有更多关于挂载传播的信息。

### 例子

```json
"rootfsPropagation": "slave",
```

## <a name="configLinuxMaskedPaths" />Masked Paths

**`maskedPaths`** (字符串数组，OPTIONAL) 将掩盖容器内提供的路径，使其不能被读取。
这些值必须是 [容器命名空间](glossary.md#container_namespace) 中的绝对路径。

### 例子

```json
"maskedPaths": [
    "/proc/kcore"
]
```

## <a name="configLinuxReadonlyPaths" />Readonly Paths

**`readonlyPaths`** (字符串数组，OPTIONAL) 将把提供的路径设置为容器内的只读。
这些值必须是 [容器命名空间](glossary.md#container-namespace)中的绝对路径。

### 例子

```json
"readonlyPaths": [
    "/proc/sys"
]
```

## <a name="configLinuxMountLabel" />Mount Label

**`mountLabel`** (string, OPTIONAL) 将为容器中的挂载设置 Selinux 上下文。

### 例子

```json
"mountLabel": "system_u:object_r:svirt_sandbox_file_t:s0:c715,c811"
```

## <a name="configLinuxPersonality" />Personality

**`personality`** (object, OPTIONAL) 设置 Linux execution personality。关于更多信息
参见 [personality][personality.2] 系统调用文档。由于大部分的选项都是
由于大多数选项已经过时，很少使用，而且有些选项会降低安全性，目前支持的选项集只是一小部分
的子集。

**`domain`** *(string, REQUIRED)* - 执行域。
    有效的常量列表如下所示。`LINUX32` 将设置 `uname` 系统调用以显示
    一个 32 位的 CPU 类型，如 `i686`。
    * `LINUX`
    * `LINUX32`

**`flags`** *(字符串数组，可选)* - 要应用的额外标志。
    目前不支持任何标志值。

[cgroup-v1]: https://www.kernel.org/doc/Documentation/cgroup-v1/cgroups.txt
[cgroup-v1-blkio]: https://www.kernel.org/doc/Documentation/cgroup-v1/blkio-controller.txt
[cgroup-v1-cpusets]: https://www.kernel.org/doc/Documentation/cgroup-v1/cpusets.txt
[cgroup-v1-devices]: https://www.kernel.org/doc/Documentation/cgroup-v1/devices.txt
[cgroup-v1-hugetlb]: https://www.kernel.org/doc/Documentation/cgroup-v1/hugetlb.txt
[cgroup-v1-memory]: https://www.kernel.org/doc/Documentation/cgroup-v1/memory.txt
[cgroup-v1-net-cls]: https://www.kernel.org/doc/Documentation/cgroup-v1/net_cls.txt
[cgroup-v1-net-prio]: https://www.kernel.org/doc/Documentation/cgroup-v1/net_prio.txt
[cgroup-v1-pids]: https://www.kernel.org/doc/Documentation/cgroup-v1/pids.txt
[cgroup-v1-rdma]: https://www.kernel.org/doc/Documentation/cgroup-v1/rdma.txt
[cgroup-v2]: https://www.kernel.org/doc/Documentation/cgroup-v2.txt
[devices]: https://www.kernel.org/doc/Documentation/admin-guide/devices.txt
[devpts]: https://www.kernel.org/doc/Documentation/filesystems/devpts.txt
[file]: http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap03.html#tag_03_164
[libseccomp]: https://github.com/seccomp/libseccomp
[proc]: https://www.kernel.org/doc/Documentation/filesystems/proc.txt
[seccomp]: https://www.kernel.org/doc/Documentation/prctl/seccomp_filter.txt
[sharedsubtree]: https://www.kernel.org/doc/Documentation/filesystems/sharedsubtree.txt
[sysfs]: https://www.kernel.org/doc/Documentation/filesystems/sysfs.txt
[tmpfs]: https://www.kernel.org/doc/Documentation/filesystems/tmpfs.txt

[full.4]: http://man7.org/linux/man-pages/man4/full.4.html
[mknod.1]: http://man7.org/linux/man-pages/man1/mknod.1.html
[mknod.2]: http://man7.org/linux/man-pages/man2/mknod.2.html
[namespaces.7_2]: http://man7.org/linux/man-pages/man7/namespaces.7.html
[null.4]: http://man7.org/linux/man-pages/man4/null.4.html
[personality.2]: http://man7.org/linux/man-pages/man2/personality.2.html
[pts.4]: http://man7.org/linux/man-pages/man4/pts.4.html
[random.4]: http://man7.org/linux/man-pages/man4/random.4.html
[sysctl.8]: http://man7.org/linux/man-pages/man8/sysctl.8.html
[tty.4]: http://man7.org/linux/man-pages/man4/tty.4.html
[zero.4]: http://man7.org/linux/man-pages/man4/zero.4.html
[user-namespaces]: http://man7.org/linux/man-pages/man7/user_namespaces.7.html
[intel-rdt-cat-kernel-interface]: https://www.kernel.org/doc/Documentation/x86/intel_rdt_ui.txt

---

通过 www.DeepL.com/Translator（免费版）翻译, 然后手工校对。
