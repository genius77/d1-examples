#!/bin/bash
# ============================================================
# D1 即调即走任务示例 —— 系统状态检查
#
# 输出系统状态（CPU, 内存, 磁盘, 进程数）
#
# 对应 D1 动态库版本: ≥ v1.1.0
# ============================================================

set -euo pipefail

# ─── 收集信息 ─────────────────────────────────────────────────
echo "=== D1 状态检查 ===" >&2

# CPU
CPU_USAGE=$(top -bn1 | grep 'Cpu(s)' | sed 's/.*, *\([0-9.]*\)%* id.*/\1/' | awk '{print 100 - $1}')

# 内存
MEM_TOTAL=$(free -g | grep Mem | awk '{print $2}')
MEM_USED=$(free -g | grep Mem | awk '{print $3}')
MEM_AVAIL=$(free -g | grep Mem | awk '{print $7}')
MEM_PERCENT=$(echo "scale=1; $MEM_USED * 100 / $MEM_TOTAL" | bc)

# 磁盘
DISK_ROOT=$(df -h / | grep / | awk '{print $5}' | sed 's/%//g')

# 进程数
PROC_COUNT=$(ps aux | wc -l)

# Uptime
UPTIME=$(uptime | awk -F'[ ,]+' '{print $3 " " $4 " " $5}')

# ─── 输出 JSON ───────────────────────────────────────────────
cat <<EOF
{
    "success": true,
    "timestamp": $(date +%s),
    "cpu": {
        "usage_percent": $CPU_USAGE
    },
    "memory": {
        "total_gb": $MEM_TOTAL,
        "used_gb": $MEM_USED,
        "available_gb": $MEM_AVAIL,
        "used_percent": $MEM_PERCENT
    },
    "disk": {
        "root_used_percent": $DISK_ROOT
    },
    "processes": {
        "count": $PROC_COUNT
    },
    "uptime": "$UPTIME"
}
EOF