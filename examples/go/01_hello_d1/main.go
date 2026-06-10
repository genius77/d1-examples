// 示例: D1 Hello World (Go)
//
// 本示例演示 D1 SDK 的基础使用流程:
//   1. 获取版本信息
//   2. 初始化 D1 运行时
//   3. 设置默认消息处理器
//   4. 启动 D1
//   5. 阻塞等待退出（D1_Wait 内置信号处理，Ctrl+C 退出）
//
// 编译运行前请确保已将 libd1.so / libd1.dylib / d1.dll 和 d1.h 放入 deps/ 目录。
// 如果 deps/ 不在默认位置，请设置 CGO_CFLAGS 和 CGO_LDFLAGS 环境变量。
package main

import (
	"fmt"
	"log"

	d1 "github.com/genius77/d1-examples/sdk/go"
)

func main() {
	log.SetFlags(log.LstdFlags | log.Lshortfile)
	log.Println("===== D1 Hello World (Go) =====")

	// 获取 D1 全局单例
	instance := d1.Default()

	// 1. 输出版本信息
	ver := instance.Version()
	log.Printf("D1 version: %s", ver)

	// 2. 初始化 D1 运行时
	//    传空字符串使用内置默认配置
	log.Println("Initializing D1...")
	if err := instance.Init(""); err != nil {
		log.Fatalf("D1 Init failed: %v", err)
	}
	log.Println("D1 initialized")

	// 3. 设置默认消息处理器
	//    当 D1 收到未匹配到特定路由的消息时，将回调此函数
	instance.SetDefaultHandler(func(taskID uint64, msgName string, payload []byte) ([]byte, error) {
		log.Printf("[handler] taskID=%d msgName=%s payload=%s", taskID, msgName, string(payload))
		return []byte(fmt.Sprintf(`{"echo":"%s"}`, string(payload))), nil
	})
	log.Println("Default handler registered")

	// 4. 启动 D1 运行时
	log.Println("Starting D1...")
	if err := instance.Start(); err != nil {
		log.Fatalf("D1 Start failed: %v", err)
	}
	log.Println("D1 running (press Ctrl+C to exit)")

	// 5. 阻塞等待退出
	//    D1_Wait 内部已监听 SIGINT/SIGTERM，用户按 Ctrl+C 即可优雅退出
	instance.Wait()

	log.Println("D1 exited")
}