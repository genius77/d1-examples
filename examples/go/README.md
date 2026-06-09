# D1 示例 — Go 语言

本目录包含 D1 动态库的 Go 语言示例程序。

---

## 环境要求

| 依赖 | 最低版本 |
|------|----------|
| Go | >= 1.21 |
| CGO | 已启用（`CGO_ENABLED=1`） |
| GCC / Clang | 编译 C 代码所需 |

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

> **提示:** 可以使用 `make setup-deps` 快速创建 `deps/` 目录。

### 3. 编译示例

进入 `01_hello_d1` 示例目录:

```bash
cd examples/go/01_hello_d1
make build
```

如果动态库不在默认位置，可手动设置 CGO 环境变量:

```bash
CGO_CFLAGS="-I/path/to/deps"  \
CGO_LDFLAGS="-L/path/to/deps -ld1" \
CGO_ENABLED=1 go build -o hello_d1 .
```

### 4. 运行

```bash
./hello_d1
```

输出示例:

```
2024/01/01 12:00:00 ===== D1 Hello World (Go) =====
2024/01/01 12:00:00 D1 动态库版本: v1.1.0
2024/01/01 12:00:00 正在初始化 D1 (配置文件: "")...
2024/01/01 12:00:00 D1 初始化成功
2024/01/01 12:00:00 默认消息处理器已设置
2024/01/01 12:00:00 正在启动 D1...
2024/01/01 12:00:00 D1 启动成功，正在运行... (按 Ctrl+C 退出)
^C
2024/01/01 12:00:05 收到信号: interrupt，正在停止 D1...
2024/01/01 12:00:05 D1 已正常退出，示例结束
```

---

## 示例列表

| 目录 | 说明 |
|------|------|
| `01_hello_d1/` | 最简示例：初始化 -> 设置处理器 -> 启动 -> 等待退出 |

---

## SDK 使用说明

详细 API 文档请参阅 SDK 源文件: [`../../sdk/go/d1.go`](../../sdk/go/d1.go)

### 基本用法

```go
import d1 "github.com/genius77/d1-examples/sdk/go"

func main() {
    // 获取全局单例
    d := d1.Default()

    // 输出 D1 版本
    fmt.Println("版本:", d.Version())

    // 初始化（可传入配置文件路径，空字符串使用默认配置）
    if err := d.Init(""); err != nil {
        log.Fatal(err)
    }

    // 设置默认消息处理器
    d.SetDefaultHandler(func(taskID uint64, msgName string, payload []byte) ([]byte, error) {
        log.Printf("收到消息: %s -> %s", msgName, string(payload))
        return []byte("ok"), nil
    })

    // 启动 D1
    if err := d.Start(); err != nil {
        log.Fatal(err)
    }

    // 发送消息
    d.Publish(1, "target/node", "event.test", `{"hello":"world"}`)

    // 同步调用
    resp, err := d.Call(2, 0, "target/node", "rpc.query", `{"sql":"SELECT 1"}`, 10)

    // 缓存操作
    d.CacheSet(3, "mykey", "myvalue", 3600)
    val, _ := d.CacheGet(3, "mykey")
    d.CacheDelete(3, "mykey")

    // 数据库操作
    json, _ := d.DBQuery(4, "SELECT * FROM items LIMIT 5")
    rows, _ := d.DBExec(5, "UPDATE items SET status=1 WHERE id=42")

    // 键值存储
    d.Set(6, "config.name", "myapp")
    cfg, _ := d.Get(6, "config.name")

    // 阻塞等待退出
    d.Wait()
}
```

---

## 常见问题

### Q: 编译时报 `d1.h: No such file or directory`
**A:** 请确保 `deps/d1.h` 文件存在。可通过 `make setup-deps` 创建目录后手动放入。

### Q: 运行时 `error while loading shared libraries: libd1.so`
**A:** Linux 下需确保动态库在链接器搜索路径中:
```bash
export LD_LIBRARY_PATH="$(pwd)/deps:$LD_LIBRARY_PATH"
./hello_d1
```

### Q: macOS 下 `dyld: Library not loaded`
**A:** macOS 下需设置 `DYLD_LIBRARY_PATH`:
```bash
export DYLD_LIBRARY_PATH="$(pwd)/deps:$DYLD_LIBRARY_PATH"
./hello_d1
```

### Q: CGO 未启用
**A:** 设置环境变量:
```bash
export CGO_ENABLED=1
```

---

## 许可证

D1 示例代码按仓库根目录 LICENSE 文件发布。D1 动态库本身的许可请联系 genius77。