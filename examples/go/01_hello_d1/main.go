// 示例: D1 Hello World (Go)
//
// 本示例演示 D1 SDK 的基础使用流程:
//   1. 初始化 D1 运行时
//   2. 设置默认消息处理器
//   3. 启动 D1
//   4. 等待信号优雅退出
//
// 编译运行前请确保已将 libd1.so / libd1.dylib / d1.dll 和 d1.h 放入 deps/ 目录。
// 如果 deps/ 不在默认位置，请设置 CGO_CFLAGS 和 CGO_LDFLAGS 环境变量。
package main

import (
	"fmt"
	"log"
	"os"
	"os/signal"
	"syscall"

	d1 "github.com/genius77/d1-examples/sdk/go"
)

func main() {
	log.SetFlags(log.LstdFlags | log.Lshortfile)
	log.Println("===== D1 Hello World (Go) =====")

	// 获取 D1 全局单例
	instance := d1.Default()

	// 1. 输出版本信息
	ver := instance.Version()
	log.Printf("D1 动态库版本: %s", ver)

	// 2. 初始化 D1 运行时
	//    传空字符串表示使用内置默认配置
	//    也可以传入配置文件路径，如: instance.Init("config.yaml")
	configPath := "" // 使用默认配置
	if envCfg := os.Getenv("D1_CONFIG"); envCfg != "" {
		configPath = envCfg
	}
	log.Printf("正在初始化 D1 (配置文件: %q)...", configPath)
	if err := instance.Init(configPath); err != nil {
		log.Fatalf("D1 初始化失败: %v", err)
	}
	log.Println("D1 初始化成功")

	// 3. 设置默认消息处理器
	//    当 D1 收到未匹配到特定路由的消息时，将回调此函数
	instance.SetDefaultHandler(func(taskID uint64, msgName string, payload []byte) ([]byte, error) {
		log.Printf("[处理器] 收到消息 | taskID=%d | msgName=%s | payload=%s",
			taskID, msgName, string(payload))

		// 返回响应载荷
		return []byte(fmt.Sprintf(`{"echo":"%s"}`, string(payload))), nil
	})
	log.Println("默认消息处理器已设置")

	// 4. 启动 D1 运行时
	log.Println("正在启动 D1...")
	if err := instance.Start(); err != nil {
		log.Fatalf("D1 启动失败: %v", err)
	}
	log.Println("D1 启动成功，正在运行... (按 Ctrl+C 退出)")

	// 5. 信号处理: 监听 SIGINT / SIGTERM
	sigCh := make(chan os.Signal, 1)
	signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)

	// 在单独的 goroutine 中等待信号
	go func() {
		sig := <-sigCh
		log.Printf("收到信号: %v，正在停止 D1...", sig)

		if err := instance.Stop(); err != nil {
			log.Printf("D1 停止时出错: %v", err)
		}
	}()

	// 6. 阻塞等待 D1 退出
	instance.Wait()

	log.Println("D1 已正常退出，示例结束")
}