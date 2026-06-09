# D1 示例 — Python 语言

本目录包含 D1 动态库的 Python 语言示例程序。

---

## 环境要求

| 依赖 | 最低版本 |
|------|----------|
| Python | >= 3.8 |
| 操作系统 | Linux / macOS / Windows |

无需额外安装第三方包（仅使用标准库 `ctypes`）。

**操作系统支持:**

| 系统 | 动态库文件 |
|------|-----------|
| Linux (x86_64 / aarch64) | `libd1.so` |
| macOS (x86_64 / arm64) | `libd1.dylib` |
| Windows (x86_64) | `d1.dll` |

---

## 快速开始

### 1. 克隆仓库

```bash
git clone https://github.com/genius77/d1-examples.git
cd d1-examples
```

### 2. 下载 D1 动态库

将 `d1.h` 与对应的动态库文件放入仓库根目录下的 `deps/` 文件夹中。

目录结构应为:

```
d1-examples/
├── deps/
│   ├── d1.h
│   ├── libd1.so        # Linux
│   ├── libd1.dylib     # macOS
│   └── d1.dll          # Windows
├── sdk/
│   ├── go/
│   │   ├── d1.go
│   │   └── go.mod
│   └── python/
│       └── d1.py
└── examples/
    ├── go/
    │   ├── 01_hello_d1/
    │   └── README.md
    └── python/
        ├── 01_hello_d1/
        └── README.md
```

### 3. 运行示例

进入 `01_hello_d1` 示例目录:

```bash
cd examples/python/01_hello_d1
python3 main.py
```

输出示例:

```
===== D1 Hello World (Python) =====
D1 动态库版本: v1.1.0
正在初始化 D1 (配置: 默认配置)...
D1 初始化成功
默认消息处理器已设置
正在启动 D1...
D1 启动成功，正在运行... (按 Ctrl+C 退出)
^C
收到信号: SIGINT，正在停止 D1...
D1 已正常退出，示例结束
```

### 4. 使用自定义配置

```bash
D1_CONFIG=/path/to/config.yaml python3 main.py
```

---

## 示例列表

| 目录 | 说明 |
|------|------|
| `01_hello_d1/` | 最简示例：初始化 -> 设置处理器 -> 启动 -> 等待退出 |

---

## SDK 使用说明

详细 API 文档请参阅 SDK 源文件: [`../../sdk/python/d1.py`](../../sdk/python/d1.py)

### 导入 SDK

```python
import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'sdk', 'python'))
from d1 import D1, D1Error
```

### 基本用法（上下文管理器）

```python
with D1() as d1:
    # 输出版本
    print("版本:", d1.version())

    # 初始化
    d1.init("config.yaml")  # 或 d1.init(None) 使用默认配置

    # 设置默认消息处理器
    def my_handler(task_id, msg_name, payload):
        print(f"收到消息: {msg_name}")
        return b"ack"

    d1.set_default_handler(my_handler)

    # 启动
    d1.start()

    # 发布消息
    d1.publish(1, "target/node", "event.test", '{"hello":"world"}')

    # 同步调用
    result = d1.call(2, 0, "target/node", "rpc.query", '{"sql":"SELECT 1"}', 10)
    if result["error"] is None:
        print("响应:", result["payload"])

    # 异步请求
    def on_response(task_id, payload, error):
        if error:
            print(f"请求失败: {error}")
        else:
            print(f"异步响应: {payload}")

    d1.request(3, "target/node", "async.query", '{"q":"data"}', 10, on_response)

    # 缓存操作
    d1.cache_set(4, "mykey", "myvalue", 3600)
    val = d1.cache_get(4, "mykey")
    d1.cache_delete(4, "mykey")

    # 数据库操作
    rows_json = d1.db_query(5, "SELECT * FROM items LIMIT 5")
    affected = d1.db_exec(6, "UPDATE items SET status=1 WHERE id=42")

    # 键值存储
    d1.set(7, "config.name", "myapp")
    cfg = d1.get(7, "config.name")

    # 阻塞等待退出
    d1.wait()
```

> 退出 `with` 语句块时，会自动调用 `stop()`。

### 手动管理生命周期

```python
d1 = D1()
try:
    d1.init(None)
    d1.start()
    d1.wait()
finally:
    d1.stop()
```

---

## 常见问题

### Q: 运行时 `ImportError: cannot import name 'D1'`
**A:** 请确保 Python 的 import 路径包含 SDK 目录:
```python
import sys
sys.path.insert(0, "../../sdk/python")
```

### Q: `FileNotFoundError: 找不到 D1 动态库文件`
**A:** 请确保 `deps/` 目录中存在对应平台的动态库文件:
- Linux: `deps/libd1.so`
- macOS: `deps/libd1.dylib`
- Windows: `deps/d1.dll`

### Q: Linux 下运行时 `libd1.so: cannot open shared object file`
**A:** 设置 `LD_LIBRARY_PATH`:
```bash
export LD_LIBRARY_PATH="$(pwd)/deps:$LD_LIBRARY_PATH"
python3 main.py
```

### Q: macOS 下 `dyld: Library not loaded`
**A:** 设置 `DYLD_LIBRARY_PATH`:
```bash
export DYLD_LIBRARY_PATH="$(pwd)/deps:$DYLD_LIBRARY_PATH"
python3 main.py
```

### Q: Windows 下找不到 DLL
**A:** 将 `deps/` 目录添加到 `PATH` 或与 `.exe` 放在同一目录。

---

## 许可证

D1 示例代码按仓库根目录 LICENSE 文件发布。D1 动态库本身的许可请联系 genius77。