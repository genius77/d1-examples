# Changelog

本文档记录 d1-examples 仓库的所有重要变更。

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)，版本号遵循 [Semantic Versioning](https://semver.org/lang/zh-CN/)。

---

## [0.1.0] — 2026-06-09

### 新增 ✨

- **SDK 封装层** (`sdk/`)：为 6 种语言提供 D1 全部 17 个 C API 的封装
  - C 头文件 (`sdk/c/d1.h`)
  - C++ RAII 封装 (`sdk/cpp/d1.hpp`)
  - Go cgo 封装 (`sdk/go/d1.go`)
  - Python ctypes 封装 (`sdk/python/d1.py`)
  - C# P/Invoke 封装 (`sdk/csharp/D1.cs`)
  - Java JNA 封装 (`sdk/java/D1.java`)

- **入门示例** (`examples/`)：每种语言一个最小可运行示例
  - `examples/c/01_hello_d1/` — C 语言入门
  - `examples/cpp/01_hello_d1/` — C++ 入门
  - `examples/go/01_hello_d1/` — Go 入门
  - `examples/python/01_hello_d1/` — Python 入门
  - `examples/csharp/01_hello_d1/` — C# 入门
  - `examples/java/01_hello_d1/` — Java 入门

- **工具脚本** (`scripts/`)
  - `download_d1.sh` — Unix/Linux/macOS 一键下载 D1 动态库
  - `download_d1.ps1` — Windows 一键下载 D1 动态库

- **示例配置文件** (`config/`)
  - `d1.yaml` / `connector.yaml` / `router.yaml` 等 D1 配置示例

- **仓库基础设施**
  - `VERSION` — 当前仓库版本
  - `CHANGELOG.md` — 变更日志
  - `README.md` — 项目说明与快速开始指南

### 依赖

- 对应 D1 动态库版本：**≥ v1.1.0**

---

[0.1.0]: https://github.com/genius77/d1-examples/releases/tag/v0.1.0