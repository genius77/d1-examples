#!/usr/bin/env python3
# ============================================================
# D1 常驻微服务示例 —— 规则引擎
#
# D1 启动时拉起此进程，通过 gRPC 或 stdio 与 D1 通信。
# 本示例展示最简单的 stdio 模式：从 stdin 读取消息，处理后写到 stdout。
#
# 对应 D1 动态库版本: ≥ v1.1.0
# ============================================================

import json
import sys
import signal
import os
from datetime import datetime

# ─── 信号处理 ───────────────────────────────────────────
def on_shutdown(signum, frame):
    sys.stderr.write(f"规则引擎收到信号 {signum}，正在退出...\n")
    sys.exit(0)

signal.signal(signal.SIGTERM, on_shutdown)
signal.signal(signal.SIGINT, on_shutdown)

# ─── 规则定义 ───────────────────────────────────────────
RULES = {
    "risk_score": {
        "condition": lambda d: d.get("amount", 0) > 10000,
        "action": "flag_review",
        "message": "交易金额超过 10000，需要人工审核"
    },
    "fraud_check": {
        "condition": lambda d: d.get("ip_country") != d.get("card_country"),
        "action": "block",
        "message": "IP 所在国与银行卡所在国不匹配"
    },
    "velocity": {
        "condition": lambda d: d.get("recent_tx_count", 0) > 50,
        "action": "flag_review",
        "message": "最近1小时交易次数超过 50 次"
    },
}

# ─── 主循环 ─────────────────────────────────────────────
sys.stderr.write(f"规则引擎已启动, PID: {os.getpid()}\n")

for line in sys.stdin:
    try:
        request = json.loads(line.strip())
    except json.JSONDecodeError:
        continue

    event = request.get("event", {})
    hits = []

    for rule_name, rule in RULES.items():
        if rule["condition"](event):
            hits.append({
                "rule": rule_name,
                "action": rule["action"],
                "message": rule["message"]
            })

    # ─── 输出结果 ──────────────────────────────────────
    response = {
        "success": True,
        "timestamp": datetime.now().isoformat(),
        "rules_checked": len(RULES),
        "hits": len(hits),
        "results": hits
    }

    sys.stdout.write(json.dumps(response) + "\n")
    sys.stdout.flush()