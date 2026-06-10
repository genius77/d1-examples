#!/bin/bash
# ============================================================
# D1 即调即走任务示例 —— 默认消息处理器
#
# 当 D1 收到未匹配到路由的消息时，可以配置调用这个 Shell 脚本。
# D1 会将 JSON 格式的输入通过 stdin 传入，输出 JSON 结果到 stdout。
#
# 调用方式：D1 会自动执行此脚本，并等待它返回。
# 对应 D1 动态库版本: ≥ v1.1.0
# ============================================================

set -euo pipefail

# ─── 读取输入（JSON from stdin） ─────────────────────────────
INPUT=$(cat)
TASK_ID=$(echo "$INPUT" | python3 -c 'import json,sys; d=json.load(sys.stdin); print(d.get("taskId",0))')
MSG_NAME=$(echo "$INPUT" | python3 -c 'import json,sys; d=json.load(sys.stdin); print(d.get("msgName",""))')
PAYLOAD=$(echo "$INPUT" | python3 -c 'import json,sys; d=json.load(sys.stdin); p=d.get("payload",{}); print(json.dumps(p))')

echo "=== D1 Shell 扩展示例 ===" >&2
echo "taskId: $TASK_ID" >&2
echo "msgName: $MSG_NAME" >&2

# ─── 你的业务逻辑 ─────────────────────────────────────────────
# 这里可以是任何 Shell 命令、管道、Python/Ruby/Perl 脚本...

case "$MSG_NAME" in
    "ping")
        # 心跳检测
        cat <<EOF
{
    "success": true,
    "result": "pong",
    "serverTime": $(date +%s)
}
EOF
        exit 0
        ;;
    "echo")
        # 回显输入
        cat <<EOF
{
    "success": true,
    "echo": $PAYLOAD,
    "length": $(echo "$PAYLOAD" | wc -c)
}
EOF
        exit 0
        ;;
    "backup")
        # 备份示例
        BACKUP_DIR="./backups"
        mkdir -p "$BACKUP_DIR"
        BACKUP_FILE="$BACKUP_DIR/backup_$(date +%Y%m%d_%H%M%S).tar.gz"

        tar -czf "$BACKUP_FILE" ./data/ 2>&1 | head -5 >&2
        SIZE=$(du -h "$BACKUP_FILE" | cut -f1)

        cat <<EOF
{
    "success": true,
    "backup_file": "$BACKUP_FILE",
    "size": "$SIZE"
}
EOF
        exit 0
        ;;
    *)
        # 默认处理
        cat <<EOF
{
    "success": false,
    "error": "unsupported_message",
    "message": "消息类型 '$MSG_NAME' 不被支持"
}
EOF
        exit 1
        ;;
esac