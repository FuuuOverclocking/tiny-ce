# Open Container Initiative Runtime Specification

开放容器倡议为操作系统进程和应用容器的标准制定规范。

# Abstract

Open Container Initiative Runtime Specification 旨在指定容器的配置、执行环境和生命周期。

容器的配置被指定为支持平台的 `config.json`，并详细说明了创建容器所需的字段。
执行环境的指定是为了确保在容器内运行的应用程序在不同的运行时间之间有一个一致的环境，以及为容器的生命周期定义的共同动作。

# <a name="ociRuntimeSpecPlatforms" />Platforms

本规范所定义的平台有。

* `linux`: [runtime.md](runtime.md), [config.md](config.md), [config-linux.md](config-linux.md), 和 [runime-linux.md](runime-linux.md)。

> 未翻译:
> 
> * `solaris`: [runtime.md](runtime.md), [config.md](config.md), 以及 [config-solaris.md](config-solaris.md)。
> * `windows`: [runtime.md](runtime.md), [config.md](config.md), 以及 [config-windows.md](config-windows.md)。
> * `vm`: [runtime.md](runtime.md), [config.md](config.md), 以及 [config-vm.md](config-vm.md)。
> * `zos`: [runtime.md](runtime.md), [config.md](config.md), 及 [config-zos.md](config-zos.md)。

# <a name="ociRuntimeSpecTOC" />Table of Contents

- 简介
    - [符号约定](#notational-conventions)
    - 容器原则
- [文件系统包](bundle.md)
- [运行时和生命周期](runtime.md)
    - [Linux 特定的运行时和生命周期](runtime-linux.md)
- [配置](config.md)
    - [Linux 专用配置](config-linux.md)
    - Solaris 专用配置
    - Windows 专用配置
    - 虚拟机专用配置
    - z/OS专用配置
- [术语](glossary.md)

# <a name="ociRuntimeSpecNotationalConventions" />符号约定

关键词 "MUST"、"MUST NOT"、"REQUIRED"、"SHALL"、"SHALL NOT"、"SHOULD"、"RECOMMENDED"、"NOT RECOMMENDED"、"MAY "和 "OPTIONAL "应按 [RFC 2119] [rfc2119] 中的解释来说明。

关键词 "未指定"、"未定义 "和 "实现定义 "应按照[C99标准的原理][c99-unspecified]中的描述来解释。

如果一个实现不能满足它所实现的[平台](#platforms)的一个或多个 MUST、REQUIRED 或SHALL 要求，那么它就不符合特定CPU架构的要求。
如果一个实现满足它所实现的[平台](#platforms)的所有 MUST、REQUIRED 和 SHALL 要求，那么它就符合特定CPU架构的要求。

[c99-unspecified]: http://www.open-std.org/jtc1/sc22/wg14/www/C99RationaleV5.10.pdf#page=18
[OCI]: http://www.opencontainers.org
[rfc2119]: http://tools.ietf.org/html/rfc2119

---

通过 www.DeepL.com/Translator（免费版）翻译, 然后手工校对。