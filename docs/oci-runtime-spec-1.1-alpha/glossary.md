# <a name=" glossary" />Glossary

## <a name=" glossaryBundle" />Bundle

一个[目录结构](bundle.md)，提前写好，分发出去，用来作为创建[容器](#container)并在其中启动一个进程的运行时的种子。

## <a name=" glossaryConfiguration" />Configuration

在 [bundle](#bundle) 中的 [`config.json`](config.md) 文件，定义了预期的 [container](#container) 和容器进程。

## <a name=" glossaryContainer" />Container

一个执行进程的环境，具有可配置的隔离和资源限制。
例如，命名空间、资源限制和挂载都是容器环境的一部分。

## <a name=" glossaryContainerNamespace" /> 容器命名空间

在 Linux 上，[配置的进程](config.md#process) 执行的 [namespaces][namespaces.7] 。

## <a name=" glossaryJson" />JSON

所有配置的 [JSON][] 都必须以 [UTF-8][] 编码。
JSON 对象必须不包括重复的名字。
JSON对象中的条目顺序并不重要。

## <a name=" glossaryRuntime" />Runtime

本规范的一个实现。
它从[bundle](#bundle)中读取[配置文件](#configuration)，使用这些信息创建一个[容器](#container)，在容器中启动一个进程，并执行其他[生命周期的动作](runtime.md)。

## <a name=" glossaryRuntimeNamespace" />运行时命名空间

在 Linux 上，从该命名空间 [创建](config-linux.md#namespaces) 新的 [容器命名空间](#container-namespace)，一些配置的资源也是从这里访问的。

[JSON]: https://tools.ietf.org/html/rfc8259
[UTF-8]: http://www.unicode.org/versions/Unicode8.0.0/ch03.pdf

[namespaces.7]: http://man7.org/linux/man-pages/man7/namespaces.7.html

---

通过 www.DeepL.com/Translator（免费版）翻译, 然后手工校对。