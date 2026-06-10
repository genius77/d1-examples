/**
 * D1 数据转换脚本
 *
 * 用于在消息路由中对 payload 进行清洗、转换、校验。
 * 可配置在 router.yaml 的 transform 钩子中。
 *
 * 对应 D1 动态库版本: ≥ v1.1.0
 */

// input 由 D1 传入，包含原始消息数据
var data = input.data || input.payload || {};

// ─── 数据校验 ──────────────────────────────────────────────
if (!data.userId) {
    d1.reply({ error: "缺少必填字段 userId" });
    // 脚本提前结束
}

if (typeof data.amount !== "number" || data.amount <= 0) {
    d1.reply({ error: "amount 必须为正数" });
}

// ─── 数据清洗 ──────────────────────────────────────────────
// 去除首尾空格
if (typeof data.name === "string") {
    data.name = data.name.trim();
}

// 字符串转义（防止注入）
if (typeof data.notes === "string") {
    data.notes = data.notes
        .replace(/\\/g, "\\\\")
        .replace(/'/g, "\\'")
        .replace(/"/g, '\\"');
}

// ─── 数据增强 ──────────────────────────────────────────────
// 从缓存中补充用户信息
var cached = d1.cacheGet("user:" + data.userId);
if (cached) {
    var profile = JSON.parse(cached);
    data.userName = profile.name;
    data.userRole = profile.role;
}

// 从临时存储中获取会话上下文
var session = d1.get("session:" + data.userId);
if (session) {
    data.sessionContext = JSON.parse(session);
}

// ─── 时间戳 ────────────────────────────────────────────────
data.transformAt = new Date().toISOString();
data.transformBy = "data_transform.js";

// ─── 记录转换日志 ──────────────────────────────────────────
d1.log("info", "数据转换完成, userId=" + data.userId + ", fields=" +
    Object.keys(data).length);

// 返回转换后的数据
data;