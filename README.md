# D1 SDK — 多语言使用示例与封装

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![D1 Version](https://img.shields.io/badge/D1-%E2%89%A5%20v1.1.0-blue)](https://github.com/genius77/D1/releases)

## 概述

本仓库是 **D1 动态库** 的官方 SDK 封装与多语言使用示例集合。D1 是一个高性能消息路由与任务调度引擎。

> ⚠️ **D1 核心源码为闭源**（位于私有仓库 [genius77/D1](https://github.com/genius77/D1)）。  
> 本仓库仅包含各语言的 SDK 封装代码和使用示例。**D1 动态库文件需从 [D1 Releases](https://github.com/genius77/D1/releases) 下载。**

## 目录结构

```
d1-examples/
├── README.md                     # 本文件
├── VERSION                       # 仓库版本号
├── CHANGELOG.md                  # 变更日志
│
├── scripts/                      # 工具脚本
│   ├── download_d1.sh            # Unix 一键下载 D1 动态库
│   └── download_d1.ps1           # Windows 一键下载 D1 动态库
│
├── config/                       # 示例配置文件
│   ├── d1.yaml                   # D1 主配置
│   ├── connector.yaml            # 连接器配置
│   ├── router.yaml               # 路由配置
│   └── ...
│
├── sdk/                          # 各语言 SDK 封装（封装 D1 全部 C API）
│   ├── c/d1.h                    # C 头文件
│   ├── cpp/d1.hpp                # C++ RAII 封装
│   ├── go/d1.go                  # Go cgo 封装
│   ├── python/d1.py              # Python ctypes 封装
│   ├── csharp/D1.cs              # C# P/Invoke 封装
│   └── java/D1.java              # Java JNA 封装
│
└── examples/                     # 可运行示例
    ├── c/01_hello_d1/            # C 语言入门
    ├── cpp/01_hello_d1/          # C++ 入门
    ├── go/01_hello_d1/           # Go 入门
    ├── python/01_hello_d1/       # Python 入门
    ├── csharp/01_hello_d1/       # C# 入门
    └── java/01_hello_d1/         # Java 入门
```

## 快速开始

### 1. Clone 仓库

```bash
git clone https://github.com/genius77/d1-examples.git
cd d1-examples
```

### 2. 下载 D1 动态库

```bash
# Unix/Linux/macOS
./scripts/download_d1.sh

# Windows PowerShell
.\scripts\download_d1.ps1
```

> 下载后动态库和头文件位于项目根目录的 `deps/` 下。

### 3. 选择语言开始

| 语言 | SDK 封装 | 入门示例 | 构建方式 |
|------|----------|----------|----------|
| C | [`sdk/c/d1.h`](sdk/c/d1.h) | [`examples/c/01_hello_d1/`](examples/c/01_hello_d1/) | `cmake -B build && cmake --build build` |
| C++ | [`sdk/cpp/d1.hpp`](sdk/cpp/d1.hpp) | [`examples/cpp/01_hello_d1/`](examples/cpp/01_hello_d1/) | `cmake -B build && cmake --build build` |
| Go | [`sdk/go/d1.go`](sdk/go/d1.go) | [`examples/go/01_hello_d1/`](examples/go/01_hello_d1/) | `go build` |
| Python | [`sdk/python/d1.py`](sdk/python/d1.py) | [`examples/python/01_hello_d1/`](examples/python/01_hello_d1/) | `python main.py` |
| C# | [`sdk/csharp/D1.cs`](sdk/csharp/D1.cs) | [`examples/csharp/01_hello_d1/`](examples/csharp/01_hello_d1/) | `dotnet run` |
| Java | [`sdk/java/D1.java`](sdk/java/D1.java) | [`examples/java/01_hello_d1/`](examples/java/01_hello_d1/) | `mvn compile exec:java` |

## D1 动态库版本依赖

| 本仓库版本 | 最低 D1 动态库版本 | 说明 |
|-----------|--------------------|------|
| 0.1.0 | ≥ v1.1.0 | 初始版本 |

> 每个 `sdk/<lang>/d1.{ext}` 文件头部均标注了对应的 D1 动态库最低版本要求。

## 贡献

欢迎提交 Issue 和 Pull Request！

- 新增某语言示例 → 在 `examples/<lang>/` 下创建新目录
- SDK 封装改进 → 修改 `sdk/<lang>/` 下对应文件
- 文档优化 → 编辑对应 README.md

## License

本仓库示例代码和 SDK 封装使用 MIT License。D1 核心动态库有独立的商业许可。