#!/bin/bash
# ============================================================
# D1 即调即走任务示例 —— CSV → JSON 转换
#
# 将 CSV 数据转换为 JSON 数组
#
# 对应 D1 动态库版本: ≥ v1.1.0
# ============================================================

set -euo pipefail

INPUT=$(cat)

# 需要 python3 解析 JSON
if ! which python3 >/dev/null; then
    cat <<EOF
{
    "success": false,
    "error": "python3_not_found",
    "message": "需要安装 python3 才能运行此示例"
}
EOF
    exit 1
fi

# ─── 从 JSON 中提取 CSV 数据 ─────────────────────────────────
CSV_DATA=$(echo "$INPUT" | python3 -c '''
import json, sys
try:
    data = json.load(sys.stdin)
    csv = data.get("csv", "")
    print(csv)
except Exception as e:
    print("")
''')

if [ -z "$CSV_DATA" ]; then
    cat <<EOF
{
    "success": false,
    "error": "no_csv_data",
    "message": "输入中找不到 csv 字段"
}
EOF
    exit 1
fi

# ─── 转换 CSV → JSON ──────────────────────────────────────
python3 <<'EOF'
import csv
import json
import sys

# Read from stdin
csv_data = sys.stdin.read()
rows = []

reader = csv.DictReader(csv_data.splitlines())
for row in reader:
    rows.append(row)

print(json.dumps({
    "success": True,
    "count": len(rows),
    "data": rows
}, indent=2))
EOF <<<"$CSV_DATA"