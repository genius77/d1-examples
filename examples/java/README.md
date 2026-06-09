# D1 SDK — Java 快速入门指南

## 1. 环境要求

- **JDK 11** 或更高版本
- **Apache Maven 3.6+**（用于构建和依赖管理）
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
│   └── java/
│       └── D1.java       # Java SDK 封装（JNA）
└── examples/
    └── java/
        └── 01_hello_d1/  # 入门示例
```

## 3. 构建并运行示例

```bash
# 进入 Java 示例目录
cd examples/java/01_hello_d1/

# 编译（含 SDK 源码）
mvn compile

# 运行示例
mvn exec:java

# 或打包为 JAR
mvn package
```

## 4. 预期输出

程序运行后将依次输出：

```
=== D1 Java 入门示例: Hello D1 ===
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

### 5.1 添加依赖

在 `pom.xml` 中添加 JNA 依赖：

```xml
<dependency>
    <groupId>net.java.dev.jna</groupId>
    <artifactId>jna</artifactId>
    <version>5.14.0</version>
</dependency>
```

### 5.2 添加 SDK 源码

将 `sdk/java/D1.java` 复制到你的源码目录下，确保包路径为 `com/genius77/d1/D1.java`。

### 5.3 设置动态库搜索路径

SDK 会自动搜索以下目录（按优先级）：
- `./deps/`
- `../deps/`
- `../../deps/`
- `../../../deps/`

如需手动指定，使用 JVM 参数：

```bash
# Linux / macOS
mvn exec:java -Djna.library.path=/path/to/deps

# Windows
mvn exec:java "-Djna.library.path=C:\path\to\deps"
```

## 6. 快速示例代码

```java
import com.genius77.d1.D1;

public class QuickStart {
    public static void main(String[] args) throws Exception {
        // 输出版本号
        System.out.println(D1.version());

        // 初始化并启动
        D1.init(null);
        D1.start();

        // 设置请求处理器
        D1.setDefaultHandler((taskId, msgName, payload) -> {
            System.out.println("收到消息: " + msgName);
            return new Object[]{"{\"ok\":true}", null, 0};
        });

        // 保持运行
        System.in.read();

        // 停止
        D1.stop();
    }
}
```

## 7. SDK 全部 API 概览

| 分类 | 方法 | 说明 |
|------|------|------|
| 生命周期 | `version()` | 获取 D1 版本号 |
| | `init(configPath)` | 初始化运行时 |
| | `start()` | 启动运行时 |
| | `stop()` | 停止运行时 |
| | `waitFor()` | 阻塞等待退出 |
| 消息处理 | `setDefaultHandler(handler)` | 设置默认请求处理器 |
| | `publish(taskId, target, msgName, payload)` | 发布消息 |
| | `call(taskId, kind, target, msgName, payload, timeoutSec)` | 同步调用 |
| | `request(taskId, target, msgName, payload, timeoutSec, callback)` | 异步请求 |
| | `reply(taskId, msgName, payload)` | 回复请求 |
| 缓存 | `cacheGet(taskId, key)` | 读取缓存 |
| | `cacheSet(taskId, key, value, ttlSeconds)` | 写入缓存 |
| | `cacheDelete(taskId, key)` | 删除缓存 |
| 数据库 | `dbQuery(taskId, query)` | 执行查询 |
| | `dbExec(taskId, query)` | 执行写操作 |
| 键值存储 | `get(taskId, key)` | 获取值 |
| | `set(taskId, key, value)` | 设置值 |

## 8. 故障排除

| 问题 | 解决方案 |
|------|----------|
| `无法加载 D1 动态库` | 确认动态库文件已放入仓库根目录 `deps/` 中，且文件名与平台匹配 |
| `java.lang.UnsatisfiedLinkError` | 检查动态库架构是否与 JDK 架构一致（32 位 vs 64 位） |
| Linux: `libD1.so: cannot open shared object file` | 运行 `export LD_LIBRARY_PATH=$(pwd)/deps:$LD_LIBRARY_PATH` |
| macOS: 安全提示阻止加载 | 在「系统偏好设置 > 安全性与隐私」中允许，或运行 `xattr -d com.apple.quarantine deps/libD1.dylib` |
| Windows: `D1.dll` 加载失败 | 确认 Visual C++ 运行库已安装，或将 `deps/` 加入 `PATH` |
| JNA 类找不到 | 确认 JNA 依赖已添加到 pom.xml，运行 `mvn dependency:resolve`