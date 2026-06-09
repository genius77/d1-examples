# D1 SDK — C# 快速入门指南

## 1. 环境要求

- **.NET SDK 8.0** 或更高版本
- 操作系统: Windows、Linux 或 macOS
- D1 动态库文件（由 d1-examples 仓库管理员提供，或从 [D1 私有仓库](https://github.com/genius77/D1) 获取）

## 2. 下载 D1 动态库

根据操作系统，将对应的动态库文件放入仓库根目录的 `deps/` 文件夹：

```
d1-examples/
├── deps/
│   ├── libD1.so          # Linux
│   ├── libD1.dylib       # macOS
│   └── D1.dll            # Windows
├── sdk/
│   └── csharp/
│       └── D1.cs         # C# SDK 封装
└── examples/
    └── csharp/
        └── 01_hello_d1/  # 入门示例
```

## 3. 生成并运行示例

```bash
# 进入 C# 示例目录
cd examples/csharp/01_hello_d1/

# 编译项目
dotnet build

# 运行示例
dotnet run
```

## 4. 预期输出

程序运行后将依次输出：

```
=== D1 入门示例: Hello D1 ===
D1 动态库版本: 1.x.x
D1 初始化成功
D1 启动成功
已注册默认请求处理器

按 Enter 键退出...
```

按 Enter 后:

```
D1 已停止
程序退出
```

## 5. 在你的项目中使用 D1 SDK

将 `sdk/csharp/D1.cs` 文件复制到你的项目中，然后在 `.csproj` 中添加：

```xml
<ItemGroup>
  <Compile Include="D1.cs" />
</ItemGroup>
```

确保动态库文件在运行时可被 .NET 发现：

- **Linux / macOS**: 将 `libD1.so` / `libD1.dylib` 放在可执行文件同级目录，或设置 `LD_LIBRARY_PATH` / `DYLD_LIBRARY_PATH`
- **Windows**: 将 `D1.dll` 放在可执行文件同级目录，或加入 `PATH`

## 6. 快速示例代码

```csharp
using Genius77.D1;

// 获取版本
Console.WriteLine(D1.Version());

// 初始化并启动
D1.Init(null);
D1.Start();

// 设置请求处理器
D1.SetDefaultHandler((taskId, msgName, payload, out var outPayload, out var outError) =>
{
    outPayload = "{\"ok\":true}";
    outError = null;
    return 0;
});

// 保持运行
Console.ReadLine();

// 停止
D1.Stop();
```

## 7. SDK 全部 API 概览

| 分类 | 方法 | 说明 |
|------|------|------|
| 生命周期 | `Version()` | 获取 D1 版本号 |
| | `Init(configPath)` | 初始化运行时 |
| | `Start()` | 启动运行时 |
| | `Stop()` | 停止运行时 |
| | `Wait()` | 阻塞等待退出 |
| 消息处理 | `SetDefaultHandler(handler)` | 设置默认请求处理器 |
| | `Publish(taskId, target, msgName, payload)` | 发布消息 |
| | `Call(taskId, kind, target, msgName, payload, timeoutSec)` | 同步调用 |
| | `Request(taskId, target, msgName, payload, timeoutSec, callback)` | 异步请求 |
| | `Reply(taskId, msgName, payload)` | 回复请求 |
| 缓存 | `CacheGet(taskId, key)` | 读取缓存 |
| | `CacheSet(taskId, key, value, ttlSeconds)` | 写入缓存 |
| | `CacheDelete(taskId, key)` | 删除缓存 |
| 数据库 | `DBQuery(taskId, query)` | 执行查询 |
| | `DBExec(taskId, query)` | 执行写操作 |
| 键值存储 | `Get(taskId, key)` | 获取值 |
| | `Set(taskId, key, value)` | 设置值 |

## 8. 故障排除

| 问题 | 解决方案 |
|------|----------|
| `DllNotFoundException: Unable to load shared library 'D1'` | 确认动态库已放入 `deps/` 目录，且文件名正确 |
| `D1 初始化失败` | 检查配置文件是否存在，或尝试传入 `null` 使用默认配置 |
| macOS 安全提示 | 在「系统偏好设置 > 安全性与隐私」中允许运行 |
| Linux 找不到库 | 运行 `export LD_LIBRARY_PATH=$(pwd)/deps:$LD_LIBRARY_PATH`