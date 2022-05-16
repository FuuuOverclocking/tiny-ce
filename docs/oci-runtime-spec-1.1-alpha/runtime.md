# <a name="runtimeAndLifecycle" />运行时和生命周期

## <a name="runtimeScopeContainer" />容器的范围

使用运行时创建容器的实体必须能够对同一容器使用本规范中定义的操作。
其他使用相同或其他运行时实例的实体是否能够看到该容器，不在本规范的范围之内。

## <a name="runtimeState" />State

容器的状态包括以下属性:

- `ociVersion` (string, REQUIRED) 是该 State 的 OCI Runtime Spec 的版本。
- `id` (string, REQUIRED) 是容器的ID。
    这个ID在这个主机上的所有容器中必须是唯一的。
    没有要求它在不同的主机上是唯一的。
- `status` (string, REQUIRED) 是容器的运行时状态。
    该值可以是以下之一。

    * `creating`: 容器正在被创建（[生命周期](#lifecycle)的第2步）。
    * `created`：运行时已经完成了[创建操作](#create)（在[生命周期](#lifecycle)的第2步之后），并且容器进程既没有退出也没有执行用户指定的程序
    * `running`: 容器进程已经执行了用户指定的程序，但是没有退出（在[lifecycle](#lifecycle)的第8步之后）。
    * `stopped`: 容器进程已经退出（[lifecycle](#lifecycle)中的第10步）。

    额外的值可以由运行时定义，但是，它们必须被用来表示上面没有定义的新的运行时状态。
- `pid` (int, REQUIRED when `status` is `created` or `running` on Linux, OPTIONAL on other platforms) 是容器进程的ID。
  对于在运行时命名空间中执行的 hook，它是运行时看到的 pid。
  对于在容器命名空间中执行的 hook，它是容器看到的 pid。
- `bundle` (string, REQUIRED) 是指向容器的 bundle 目录的绝对路径。
    这是为了让消费者能够在主机上找到容器的配置和根文件系统。
- `annotations` (map, OPTIONAL) 包含与该容器相关的注释列表。
    如果没有提供注解，那么这个属性可能是不存在的，或者是一个空 map。

状态可能包括额外的属性。

当被序列化为 JSON 时，其格式必须遵守以下模式。

```json
{
    "ociVersion": "0.2.0",
    "id": "oci-container1",
    "status": "running",
    "pid": 4422,
    "bundle": "/containers/redis",
    "annotations": {
        "myKey": "myValue"
    }
}
```

参见[查询状态](#query-state)，了解检索容器状态的信息。

## <a name="runtimeLifecycle" />Lifecycle

生命周期描述了从一个容器被创建到它不再存在所发生的事件的时间线。

1. 兼容 OCI 的 runtime 的 [`create`](runtime.md#create) 命令被调用，并提供了一个对 bundle 位置的引用和一个唯一 id。
2. 容器的运行时环境必须根据 [`config.json`](config.md) 中的配置来创建。
    如果运行时无法创建 [`config.json`](config.md) 中指定的环境，它必须 [产生一个错误](#errors) 。
    虽然[`config.json`](config.md)中要求的资源必须被创建，但用户指定的程序（来自[`process`](config.md#process)）此时必须不运行。
    在这一步之后对 [`config.json`](config.md) 的任何更新都不能影响容器。
3. 运行时必须调用 [`prestart` 钩子](config.md#prestart)。
    如果任何 `prestart` 钩子失败，运行时必须 [产生一个错误](#errors)，停止容器，并在第 12 步继续进行生命周期。
4. 运行时必须调用 [`createRuntime` 钩子](config.md#createRuntime-hooks)。
    如果任何 `createRuntime` 钩子失败，运行时必须[产生一个错误](#errors)，停止容器，并在第 12 步继续进行生命周期。
5. 运行时必须调用 [`createContainer` 钩子](config.md#createContainer-hooks)。
    如果任何 `createContainer` 钩子失败，运行时必须[产生一个错误](#errors)，停止容器，并在第12步继续进行生命周期。
6. 运行时的 [`start`](runtime.md#start) 命令被调用，并使用容器的唯一标识符。
7. 运行时必须调用 [`startContainer`钩子](config.md#startContainer-hooks)。
    如果任何 `startContainer` 钩子失败，运行时必须[生成一个错误](#errors)，停止容器，并在步骤 12 继续生命周期。
8. 运行时必须运行用户指定的程序，如[`process`](config.md#process)所指定。
9. 运行时必须调用 [`poststart`钩子](config.md#poststart)。
    如果任何 `poststart` 钩子失败，运行时必须[记录一个警告](#warnings)，但其余的钩子和生命周期继续进行，就像钩子成功了一样。
10. 容器进程退出。
    这可能是由于 erroring out, exiting, crashing 或运行时的 [`kill`](runtime.md#kill) 操作发生的。
11. 运行时的 [`delete`](runtime.md#delete) 命令被调用，并使用容器的唯一标识符。
12. 容器必须通过撤销在创建阶段（第 2 步）执行的步骤来销毁。
13. 运行时必须调用 [`poststop`钩子](config.md#poststop)。
    如果任何一个 "poststop "钩子失败，运行时必须 [记录一个警告](#warnings)，但其余的钩子和生命周期继续进行，就像钩子成功了一样。

## <a name="runtimeErrors" />Errors

在指定的操作产生错误的情况下，本规范没有规定如何，或者甚至是否，将错误返回或暴露给实现的用户。
除非另有说明，产生的错误必须使环境的状态如同该操作从未被尝试过一样--除了任何可能的微不足道的附属变化，如日志。

## <a name="runtimeWarnings" />Warnings

在指定的操作记录警告的情况下，本规范没有规定如何，或者甚至是否将该警告返回或暴露给实现的用户。
除非另有说明，记录一个警告并不改变操作的流程；它必须继续进行，就像警告没有被记录一样。

## <a name="runtimeOperations" />Operations

除非另有说明，运行时必须支持下列操作。

注意：这些操作没有指定任何命令行API，参数是一般操作的输入。

### <a name="runtimeQueryState" />查询状态

`state <container-id>`

如果没有提供一个容器的ID，这个操作必须[产生一个错误](#errors)。
试图查询一个不存在的容器，必须[产生一个错误](#errors)。
这个操作必须返回 [State](#state) 部分指定的容器的状态。

### <a name="runtimeCreate" />Create

`create <container-id> <path-to-bundle>`

如果没有提供到捆绑包的路径和与容器相关联的容器ID，这个操作必须[产生一个错误](#errors)。
如果提供的 ID 在运行时范围内的所有容器中不是唯一的，或者在任何其他方面都是无效的，那么实现必须 [生成一个错误](#errors)，而且不能创建新的容器。
这个操作必须创建一个新的容器。

在 [`config.json`](config.md) 中配置的所有属性，除了 [`process`](config.md#process) 之外都必须被应用。
[`process.args`](config.md#process)在被[`start`](#start)操作触发之前不得应用。
其余的 "进程 "属性可以由该操作应用。
如果运行时不能应用[配置](config.md)中指定的属性，它必须[产生一个错误](#errors)，并且不能创建新的容器。

在创建容器之前（[步骤 2](#lifecycle)），运行时可能会根据本规范验证 `config.json`，可以是一般性的，也可以是与本地系统能力有关的。
对创建前验证感兴趣的运行时调用者可以在调用创建操作之前运行 [bundle-validation tools](implementations.md#testing--tools) 。

在此操作之后对 [`config.json`](config.md) 文件的任何修改都不会对容器产生影响。

### <a name="runtimeStart" />Start

`start <container-id>`。

如果没有提供容器的ID，这个操作必须[产生一个错误](#errors)。
试图 "启动 "一个没有被 "创建"(#state)的容器必须对该容器没有影响，并且必须[产生一个错误](#errors)。
该操作必须运行用户指定的程序，如[`process`](config.md#process)所指定。
如果没有设置 `process`，则此操作必须产生一个错误。

### <a name="runtimeKill" />Kill

`kill <container-id> <signal>`。

如果没有提供容器ID，这个操作必须[产生一个错误](#errors)。
试图向一个既没有[`created` 也没有 `running`](#state)的容器发送信号，必须对该容器没有任何影响，而且必须[产生一个错误](#errors)。
这个操作必须向容器进程发送指定的信号。

### <a name="runtimeDelete" />Delete

`delete <container-id>`

如果没有提供容器的ID，这个操作必须[产生一个错误](#errors)。
试图 "删除 "一个没有被 "停止"(#state)的容器必须对该容器没有影响，并且必须[产生一个错误](#errors)。
删除一个容器必须删除在 "创建 "步骤中创建的资源。
请注意，与容器相关的、但不是由这个容器创建的资源决不能被删除。
一旦一个容器被删除，它的ID可能会被后来的容器使用。


## <a name="runtimeHooks" />Hooks

本规范中的许多操作都有 "钩子"，允许在每个操作之前或之后采取额外的行动。
更多信息请参见 [钩子的运行时配置](./config.md#posix-platform-hooks)。

---

通过 www.DeepL.com/Translator（免费版）翻译, 然后手工校对。