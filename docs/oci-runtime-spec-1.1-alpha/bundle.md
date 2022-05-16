# <a name="filesystemBundle" />文件系统包

## <a name="containerFormat" />容器格式

本节定义了一种将容器编码为 *filesystem bundle* 的格式 - 一组以某种方式组织的文件，包含所有必要的数据和元数据，以便任何兼容的运行时对其执行所有标准操作。
参见[MacOS应用程序捆绑][macos_bundle]，了解术语*捆绑*的类似用法。

捆绑包的定义只涉及到如何将容器及其配置数据存储在本地文件系统上，以便它可以被兼容的运行时所消耗。

一个标准的容器包包含加载和运行一个容器所需的所有信息。
这包括以下两个工件。

1. <a name="containerFormat01" />`config.json`: 包含配置数据。
    这个必须的文件必须位于捆绑目录的根部，并且必须命名为`config.json`。
    参见 [`config.json`](config.md) 了解更多细节。

2. <a name="containerFormat02" />容器的根文件系统：由 [`root.path`](config.md#root) 引用的目录，如果该属性在 `config.json` 中设置。

> 译注: 通常容器的根文件系统命名为 `rootfs`, 和 `config.json` 放在同一目录下.

当提供时，虽然这些工件必须全部存在于本地文件系统的一个目录中，但该目录本身并不是捆绑包的一部分。
换句话说，一个 *bundle* 的 tar 归档文件会在归档文件的根部有这些工件，而不是嵌套在一个顶级目录中。

[macos_bundle]: https://en.wikipedia.org/wiki/Bundle_%28macOS%29

---

通过 www.DeepL.com/Translator（免费版）翻译, 然后手工校对。