# D1 C++ 示例 —— 快速入门指南

本目录包含 D1 SDK 的 C++ 示例程序，展示如何通过 D1 C++ 封装类以现代 C++ 风格调用 D1 动态库。

---

## 前置条件

- **操作系统**: Linux (x86_64 / aarch64)、macOS (x86_64 / arm64)、Windows (x86_64)
- **编译器**: GCC ≥ 7.0、Clang ≥ 10.0、或 MSVC ≥ 2019（需支持 C++17）
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

> **注意**: 如果您的平台上没有自动下载脚本，请手动从 D1 官方发布渠道获取对应文件。

---

## 第二步：编译

进入示例目录，使用 CMake 构建：

```bash
cd examples/cpp/01_hello_d1
mkdir -p build && cd build
cmake ..
cmake --build .
```

编译完成后，可执行文件 `hello_d1_cpp` 将生成在 `build/` 目录下。

### 常见问题

- **找不到 d1 库**: 确保 `deps/` 目录中存在对应平台的动态库文件，且 CMakeLists.txt 中的路径正确。
- **C++17 不支持**: 请升级编译器至 GCC 7+ 或 Clang 10+。
- **运行时找不到库**: 设置 `LD_LIBRARY_PATH`（Linux）或 `DYLD_LIBRARY_PATH`（macOS）指向 `deps/` 目录。

---

## 第三步：运行

```bash
cd examples/cpp/01_hello_d1/build

# Linux
LD_LIBRARY_PATH=../../../deps ./hello_d1_cpp

# macOS
DYLD_LIBRARY_PATH=../../../deps ./hello_d1_cpp

# 或持久设置库路径
export LD_LIBRARY_PATH=$(pwd)/../../../deps:$LD_LIBRARY_PATH
./hello_d1_cpp
```

程序将输出类似以下内容：

```
[信息] D1 版本: 1.1.0
[信息] D1::Init() 成功
[信息] 已设置默认消息处理器 (C++ lambda)
[信息] 已注册 SIGINT / SIGTERM 信号处理
[信息] D1::Start() 成功
[信息] D1 运行时已启动
[信息] D1 运行中，按 Ctrl+C 退出...
^C
[信号] 收到退出信号，正在优雅关闭...
[信息] 正在停止 D1...
[信息] D1::Stop() 成功
[信息] D1 已完全停止，程序退出。
```

---

## C++ API 速览

`D1` 类为纯静态方法类，所有调用均通过 `D1::MethodName()` 形式进行。

| 类别       | 方法                                | 说明                               |
| ---------- | ----------------------------------- | ---------------------------------- |
| 生命周期   | `D1::Version() -> string`           | 获取版本号                         |
| 生命周期   | `D1::Init(config)`                  | 初始化，失败抛 runtime_error       |
| 生命周期   | `D1::Start()`                       | 启动运行时                         |
| 生命周期   | `D1::Stop()`                        | 停止运行时                         |
| 生命周期   | `D1::Wait()`                        | 阻塞等待停止                       |
| 生命周期   | `D1::SetDefaultHandler(lambda)`     | 设置默认处理器（支持 C++ lambda）  |
| 生命周期   | `D1::Free(ptr)`                     | 释放内存                           |
| 消息       | `D1::Publish(taskId, target, msg, payload) -> int`                  | 发布消息               |
| 消息       | `D1::Call(taskId, kind, target, msg, payload, timeout) -> CallResult`| 同步调用               |
| 消息       | `D1::Request(taskId, target, msg, payload, timeout, callback) -> int`| 异步请求               |
| 消息       | `D1::Reply(taskId, msg, payload) -> int`                             | 响应请求               |
| 缓存       | `D1::CacheGet(taskId, key) -> Buffer`                                | 读取缓存               |
| 缓存       | `D1::CacheSet(taskId, key, value, ttl) -> int`                       | 写入缓存               |
| 缓存       | `D1::CacheDelete(taskId, key) -> int`                                | 删除缓存               |
| 数据库     | `D1::DBQuery(taskId, query) -> Buffer`                               | 数据库查询             |
| 数据库     | `D1::DBExec(taskId, query) -> DBExecResult`                          | 数据库变更             |
| 临时存储   | `D1::Set(taskId, key, value) -> int`                                 | 设置临时键值           |
| 临时存储   | `D1::Get(taskId, key) -> Buffer`                                     | 获取临时键值           |

### 关键类型说明

- **Buffer** —— RAII 内存管理类。自动持有 D1 分配的动态内存，析构时自动调用 `D1_Free`。支持移动语义，禁止拷贝。提供 `toString()`、`valid()`、`empty()` 等便捷方法。
- **CallResult** —— `D1::Call` 的返回结构，包含 `payload` 和 `error` 两个 Buffer 成员。`ok()` 方法判断调用是否成功。
- **DBExecResult** —— `D1::DBExec` 的返回结构，包含 `return_code` 和 `affected_rows`。

---

## C++ 封装的优势

与原始 C API 相比，D1 C++ 封装提供以下优势：

1. **RAII 自动内存管理** —— 不再需要手动调用 `D1_Free`，Buffer 析构时自动释放。
2. **异常安全** —— 初始化/启动失败时抛出 `std::runtime_error`，避免层层检查返回值。
3. **类型安全** —— 使用 `std::string` 替代 C 风格字符串，减少缓冲区溢出风险。
4. **现代 C++ 回调** —— 支持 lambda 表达式和 `std::function`，编写更简洁。
5. **零开销抽象** —— 所有封装均为内联或轻量级包装，不引入额外运行时成本。

---

## 更多示例

更多 C++ 示例请关注本仓库的后续更新。