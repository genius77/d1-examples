// D1 Java 入门示例: Hello D1
// 演示 D1 SDK 基本生命周期:
//   1. 打印版本号
//   2. 初始化 D1 (使用默认配置)
//   3. 启动 D1 运行时
//   4. 设置默认请求处理器
//   5. 等待用户输入以保持运行
//   6. 停止 D1 并退出
//
// 运行前提: 确保 D1 动态库已放入 deps/ 目录。

package com.genius77.d1.examples;

import com.genius77.d1.D1;

/**
 * D1 Java 入门示例程序。
 * <p>
 * 构建: mvn package<br>
 * 运行: mvn exec:java -Dexec.mainClass="com.genius77.d1.examples.HelloD1"
 * </p>
 */
public class HelloD1 {

    public static void main(String[] args) {
        System.out.println("=== D1 Java 入门示例: Hello D1 ===");

        try {
            // -------------------------------------------------------
            // 1. 获取并打印 D1 动态库版本号
            // -------------------------------------------------------
            String version = D1.version();
            System.out.println("D1 动态库版本: " + version);

            // -------------------------------------------------------
            // 2. 使用默认配置初始化 D1 运行时
            //    传入 null 表示使用默认配置（从 deps/ 目录读取）
            // -------------------------------------------------------
            D1.init(null);
            System.out.println("D1 初始化成功");

            // -------------------------------------------------------
            // 3. 启动 D1 运行时
            // -------------------------------------------------------
            D1.start();
            System.out.println("D1 启动成功");

            // -------------------------------------------------------
            // 4. 设置默认请求处理器
            //    所有未被路由匹配的请求都会回调此处理器
            // -------------------------------------------------------
            D1.setDefaultHandler((taskId, msgName, payload) -> {
                System.out.println("[默认处理器] 收到请求: taskId=" + taskId +
                        ", msgName=" + msgName + ", payload=" + payload);

                // 构造响应
                String response = "{\"message\":\"Hello from Java D1 SDK!\",\"taskId\":" + taskId + "}";
                System.out.println("回复请求: " + response);

                // [0]=响应负载, [1]=错误信息(null表示无), [2]=返回码(0表示成功)
                return new Object[]{response, null, 0};
            });
            System.out.println("已注册默认请求处理器");

            // -------------------------------------------------------
            // 5. 等待用户输入，保持进程运行
            // -------------------------------------------------------
            System.out.println("\n按 Enter 键退出...");
            System.in.read();

            // -------------------------------------------------------
            // 6. 停止 D1 运行时
            // -------------------------------------------------------
            D1.stop();
            System.out.println("D1 已停止");

            System.out.println("程序退出");

        } catch (D1.D1Exception e) {
            System.out.println("\nD1 操作异常: " + e.getMessage());
            System.out.println("错误码: " + e.getErrorCode() + ", 函数名称: " + e.getFunctionName());
            System.exit(1);
        } catch (Exception e) {
            System.out.println("\n未处理异常: " + e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }
}