# D1 C 语言示例 —— 快速入门指南

本目录包含 D1 SDK 的 C 语言示例程序，帮助您快速上手 D1 动态库。

---

## 前置条件

- **操作系统**: Linux (x86_64 / aarch64)、macOS (x86_64 / arm64)、Windows (x86_64)
- **编译器**: GCC ≥ 7.0、Clang ≥ 10.0、或 MSVC ≥ 2019
- **CMake**: ≥ 3.16
- **D1 动态库**: 从 D1 发布页面下载对应平台的 `libd1.so` / `libd1.dylib` / `d1.dll` 以及 `d1.h`

---

## 第一步：下载 D1 动态库

将下载的 `libd1.so`（或对应平台的动态库文件）和 `d1.h` 放置于仓库根目录下的 `deps/` 目录中：

```
d1-examples/
├── deps/
│   ├── libd1.so       # D1 动态库
│   └── d1.h           # D1 头文件（也可使用 sdk/c/d1.h）
├── sdk/
│   ├── c/d1.h
│   └── cpp/d1.hpp
└── examples/
    ├── c/
    │   ├── 01_hello_d1/
    │   │   ├── main.c
    │   │   └── CMakeLists.txt
    │   └── README.md
    └── cpp/
        ├── 01_hello_d1/
        │   ├── main.cpp
        │   └── CMakeLists.txt
        └── README.md
```

> **注意**: 如果您的平台上没有 `download_deps.sh` 脚本，请手动从 D1 官方发布渠道获取对应文件。

---

## 第二步：编译

进入示例目录，使用 CMake 构建：

```bash
cd examples/c/01_hello_d1
mkdir -p build && cd build
cmake ..
cmake --build .
```

编译完成后，可执行文件 `hello_d1_c` 将生成在 `build/` 目录下。

### 常见问题

- **找不到 d1 库**: 确保 `deps/` 目录中存在对应平台的动态库文件，且 CMakeLists.txt 中的路径正确。
- **运行时找不到库**: 设置 `LD_LIBRARY_PATH`（Linux）或 `DYLD_LIBRARY_PATH`（macOS）指向 `deps/` 目录。

---

## 第三步：运行

```bash
cd examples/c/01_hello_d1/build

# Linux / macOS
LD_LIBRARY_PATH=../../../deps ./hello_d1_c

# 或直接设置库路径
export LD_LIBRARY_PATH=$(pwd)/../../../deps:$LD_LIBRARY_PATH
./hello_d1_c
```

程序将输出类似以下内容：

```
[信息] D1 版本: 1.1.0
[信息] D1_Init() 成功
[信息] 已设置默认消息处理器
[信息] 已注册 SIGINT / SIGTERM 信号处理
[信息] D1_Start() 成功，D1 运行时已启动
[信息] D1 运行中，按 Ctrl+C 退出...
^C
[信号] 收到退出信号，正在优雅关闭...
[信息] 正在停止 D1...
[信息] D1_Stop() 成功
[信息] D1 已完全停止，程序退出。
```

---

## C API 速览

以下为 D1 C API 的简要功能表：

| 类别       | 函数                    | 说明                         |
| ---------- | ----------------------- | ---------------------------- |
| 生命周期   | `D1_Version()`          | 获取版本号                   |
| 生命周期   | `D1_Init(config_path)`  | 初始化运行时                 |
| 生命周期   | `D1_Start()`            | 启动消息循环                 |
| 生命周期   | `D1_Stop()`             | 停止运行时                   |
| 生命周期   | `D1_Wait()`             | 阻塞等待停止                 |
| 消息处理   | `D1_SetDefaultHandler()`| 设置默认请求处理器           |
| 消息       | `D1_Publish()`          | 发布（广播）消息             |
| 消息       | `D1_Call()`             | 同步远程调用                 |
| 消息       | `D1_Request()`          | 异步请求                     |
| 消息       | `D1_Reply()`            | 响应请求                     |
| 缓存       | `D1_CacheGet()`         | 读取缓存                     |
| 缓存       | `D1_CacheSet()`         | 写入缓存                     |
| 缓存       | `D1_CacheDelete()`      | 删除缓存                     |
| 数据库     | `D1_DBQuery()`          | 查询数据库（返回 JSON）      |
| 数据库     | `D1_DBExec()`           | 执行 SQL 变更                |
| 临时存储   | `D1_Set()`              | 设置临时键值                 |
| 临时存储   | `D1_Get()`              | 获取临时键值                 |
| 内存管理   | `D1_Free()`             | 释放 API 分配的内存          |

详细文档请参阅 SDK 头文件或 D1 官方文档。

---

## 更多示例

更多 C 语言示例请关注本仓库的后续更新。