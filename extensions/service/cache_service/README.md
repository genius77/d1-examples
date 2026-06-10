# cache_service — 缓存服务示例

Shell + nc 实现的最简 HTTP 缓存服务示例。

## 说明

这是一个演示项目，展示如何编写一个 D1 常驻微服务。D1 启动时拉起此服务，通过 HTTP 端口通信。

## 使用

```
# 启动
./cache_service.sh 9091

# 使用
curl http://localhost:9091/set/key/value
curl http://localhost:9091/get/key
curl http://localhost:9091/delete/key
```

## D1 配置

在 `service.yaml` 中配置：

```yaml
items:
  - name: "cache_service"
    command: "./extensions/service/cache_service/cache_service.sh"
    args: ["{{service_port}}"]
```
