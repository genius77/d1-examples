# rule_engine — 规则引擎示例

Python 实现的简单规则引擎微服务示例。

## 说明

本示例展示如何使用 Python 编写一个 D1 常驻微服务，通过 stdio 模式与 D1 通信：

- D1 将消息写入服务进程的 stdin
- Python 进程读取并计算规则命中情况
- 结果写入 stdout
- D1 读取结果继续路由分发

## 规则示例

提供三个规则：

| 规则 | 触发条件 | 动作 |
|------|----------|------|
| `risk_score` | 金额 > 10000 | 需要人工审核 |
| `fraud_check` | IP 国家 ≠ 卡国家 | 拦截 |
| `velocity` | 1小时交易 > 50笔 | 需要人工审核 |

## D1 配置

在 `services.yaml` 中配置：

```yaml
items:
  - name: "rule_engine"
    command: "python3 ./extensions/services/rule_engine/rule_engine.py"
```
