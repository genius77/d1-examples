/**
 * D1 JavaScript 扩展示例
 *
 * D1 内置 Goja (Go JavaScript 引擎)，脚本中可直接使用以下全局对象：
 *  - d1      : D1 核心 API（消息、缓存、数据库、日志）
 *  - console : 控制台输出
 *  - input   : D1 传入的输入参数（若配置了 data 字段）
 *
 * 对应 D1 动态库版本: ≥ v1.1.0
 */

// ============================================================
// 控制台输出
// ============================================================
console.log("=== D1 Script Example ===");
console.log("Input data:", JSON.stringify(input));

// ============================================================
// 日志
// ============================================================
d1.log("info", "脚本已启动");
d1.log("debug", "收到的输入: " + JSON.stringify(input));

// ============================================================
// 临时键值存储（跨调用共享）
// ============================================================
d1.set("last_script_run", new Date().toISOString());

var cachedValue = d1.get("last_script_run");
d1.log("info", "上次运行时间: " + cachedValue);

// ============================================================
// 缓存操作
// ============================================================
var cacheKey = "user:" + (input.userId || "default");
var userData = d1.cacheGet(cacheKey);

if (!userData) {
    d1.log("info", "缓存未命中，写入新数据");
    d1.cacheSet(cacheKey, JSON.stringify({
        name: "示例用户",
        role: "admin"
    }), 3600); // TTL 3600 秒
} else {
    d1.log("info", "缓存命中: " + userData);
}

// ============================================================
// 数据库操作
// ============================================================
var queryResult = d1.dbQuery("SELECT name FROM users LIMIT 5");
d1.log("info", "数据库查询结果: " + queryResult);

var affected = d1.dbExec("UPDATE tasks SET status = 'done' WHERE id = 1");
d1.log("info", "数据库更新影响行数: " + affected);

// ============================================================
// 消息发布
// ============================================================
var taskId = input.taskId || 0;

// 发布到消息队列
d1.publish("user.login", {
    userId: input.userId,
    timestamp: Date.now()
});

// 同步调用其他服务
try {
    var result = d1.call("rpc", "auth.service", "validate_token", {
        token: input.token
    }, 5000); // 超时 5 秒
    d1.log("info", "服务调用结果: " + JSON.stringify(result));
} catch (e) {
    d1.log("error", "服务调用失败: " + e.message);
}

// ============================================================
// 返回结果
// ============================================================
// D1 会取脚本最后一行表达式值作为返回值
({
    success: true,
    timestamp: Date.now(),
    message: "示例脚本执行完成"
});