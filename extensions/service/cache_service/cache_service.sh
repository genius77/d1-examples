#!/bin/bash
# ============================================================
# D1 常驻微服务示例 —— 缓存服务
#
# 这是一个最简单的 D1 常驻微服务骨架：Shell + 监听循环。
# 正式环境推荐用 Go/C++ 实现 gRPC 或 stdio 通信。
#
# 工作原理：
#   1. D1 启动时调用 ./cache_service --port <分配给服务的端口>
#   2. 服务监听指定端口，处理 D1 发来的业务请求
#   3. D1 关闭时发送 SIGTERM，服务优雅退出
#
# 对应 D1 动态库版本: ≥ v1.1.0
# ============================================================

set -euo pipefail

PORT="${1:-9091}"
PID_FILE="./cache_service.pid"
DATA_FILE="./cache_data.json"

cleanup() {
    echo "缓存服务收到退出信号，正在关闭..." >&2
    rm -f "$PID_FILE" "$DATA_FILE"
    exit 0
}

trap cleanup SIGTERM SIGINT

# ─── 初始化 ──────────────────────────────────────────────
echo "$$" > "$PID_FILE"
echo "{}" > "$DATA_FILE"
echo "缓存服务已启动，端口: $PORT, PID: $$" >&2

# ─── 最简单的 HTTP API ───────────────────────────────────
# 正式环境：将下面的 nc 循环替换为 gRPC handler
# 这里用 nc + bash 实现轻量演示

handle_request() {
    local method="$1"
    local key="$2"
    local value="${3:-}"

    case "$method" in
        GET)
            local result
            result=$(python3 -c "import json,sys; d=json.load(open('$DATA_FILE')); print(d.get('$key','').replace('\"','\\\\\"'))" 2>/dev/null || echo "")
            echo -e "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n$result"
            ;;
        SET)
            python3 -c "
import json
d=json.load(open('$DATA_FILE'))
d['$key']='$value'
json.dump(d, open('$DATA_FILE','w'))
" 2>/dev/null
            echo -e "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nOK"
            ;;
        DEL)
            python3 -c "
import json
d=json.load(open('$DATA_FILE'))
d.pop('$key',None)
json.dump(d, open('$DATA_FILE','w'))
" 2>/dev/null
            echo -e "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nDELETED"
            ;;
        *)
            echo -e "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nUNKNOWN"
            ;;
    esac
}

# ─── 主循环（监听端口） ──────────────────────────────────
while true; do
    # 用 nc 监听 HTTP 请求
    nc -l -p "$PORT" -w 1 | while read -r line; do
        line=$(echo "$line" | tr -d '\r')
        if echo "$line" | grep -q 'GET'; then
            key=$(echo "$line" | awk '{print $2}' | tr -d '/cache/')
            handle_request GET "$key"
        elif echo "$line" | grep -q 'POST'; then
            body=$(cat)
            key=$(echo "$body" | python3 -c "import json,sys; print(json.load(sys.stdin).get('key',''))")
            value=$(echo "$body" | python3 -c "import json,sys; print(json.load(sys.stdin).get('value',''))")
            handle_request SET "$key" "$value"
        elif echo "$line" | grep -q 'DELETE'; then
            key=$(echo "$line" | awk '{print $2}' | tr -d '/cache/')
            handle_request DEL "$key"
        fi
    done
done