/**
 * D1 自定义消息处理器
 *
 * 该脚本作为 D1 默认消息处理器，所有未匹配路由的消息会路由到这里。
 * 配置时在 router.yaml 中设置 default_handler 指向该脚本。
 *
 * 对应 D1 动态库版本: ≥ v1.1.0
 */

// D1 传入的消息格式：
// input = {
//     taskId: <uint64>,
//     msgName: <string>,
//     payload: <object>,
//     target: <string>,
//     kind: <string>
// }

var msgName = input.msgName || "unknown";
var payload = input.payload || {};

console.log("收到消息:", msgName);
d1.log("info", "处理消息 [" + msgName + "], taskId=" + input.taskId);

// ─── 根据消息名称分发 ──────────────────────────────────────
switch (msgName) {

    case "ping":
        // 心跳检测 → 直接回复 pong
        d1.reply({
            status: "pong",
            serverTime: Date.now()
        });
        break;

    case "get_status":
        // 获取状态 → 从缓存读取
        var status = d1.cacheGet("system:status");
        d1.reply({
            status: status ? JSON.parse(status) : { state: "unknown" }
        });
        break;

    case "update_config":
        // 更新配置 → 写入缓存 + 数据库
        d1.cacheSet("system:config", JSON.stringify(payload), 7200);
        d1.cacheDelete("system:status"); // 使状态缓存失效

        d1.dbExec(
            "INSERT INTO config_log (key, value, updated_at) VALUES ('system', '" +
            JSON.stringify(payload).replace(/'/g, "''") + "', datetime('now'))"
        );

        d1.reply({ result: "ok", updated: Date.now() });
        break;

    case "broadcast":
        // 广播消息 → 发布到消息队列
        var topic = payload.topic || "default";
        d1.publish(topic, {
            from: "custom_handler",
            data: payload.data,
            ts: Date.now()
        });
        d1.reply({ result: "published", topic: topic });
        break;

    default:
        // 未知消息 → 返回提示
        d1.log("warn", "未识别的消息类型: " + msgName);
        d1.reply({
            error: "unsupported_message",
            msgName: msgName
        });
}

// 脚本执行完成，无需返回值（已通过 d1.reply 返回）