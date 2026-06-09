using System;
using Genius77.D1;

namespace D1HelloWorld
{
    /// <summary>
    /// D1 C# 入门示例: Hello D1
    /// 流程:
    /// 1. 打印 D1 版本号
    /// 2. 使用默认配置初始化 D1
    /// 3. 启动 D1 运行时
    /// 4. 设置默认请求处理器
    /// 5. 等待用户输入（进程保持运行）
    /// 6. 停止 D1 并退出
    /// </summary>
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("=== D1 入门示例: Hello D1 ===");

            try
            {
                // 1. 获取并打印 D1 动态库版本
                string version = D1.Version();
                Console.WriteLine($"D1 动态库版本: {version}");

                // 2. 使用默认配置初始化 D1
                // 传入 null 表示使用默认配置（从 deps/ 目录下读取）
                D1.Init(null);
                Console.WriteLine("D1 初始化成功");

                // 3. 启动 D1 运行时
                D1.Start();
                Console.WriteLine("D1 启动成功");

                // 4. 设置默认请求处理器
                // 所有未被路由匹配的请求都会路由到这里
                D1.SetDefaultHandler(OnRequest);
                Console.WriteLine("已注册默认请求处理器");

                // 5. 等待用户输入，保持进程运行
                Console.WriteLine("\n按 Enter 键退出...");
                Console.ReadLine();

                // 6. 停止 D1 运行时
                D1.Stop();
                Console.WriteLine("D1 已停止");

                Console.WriteLine("程序退出");
            }
            catch (D1Exception ex)
            {
                Console.WriteLine($"\nD1 操作异常: {ex.Message}");
                Console.WriteLine($"错误码: {ex.ErrorCode}, 函数名称: {ex.FunctionName}");
                Environment.Exit(1);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"\n未处理异常: {ex.Message}");
                Environment.Exit(1);
            }
        }

        /// <summary>
        /// 默认请求处理回调
        /// 记录请求信息并返回简单的欢迎消息
        /// </summary>
        static int OnRequest(ulong taskId, string msgName, string payload,
            out string outPayload, out string outError)
        {
            Console.WriteLine($"[默认处理器] 收到请求: taskId={taskId}, msgName={msgName}, payload={payload}");

            // 构造响应: 这里返回一个简单的欢迎 JSON
            outPayload = "{\"message\":\"Hello from C# D1 SDK!\",\"taskId\":" + taskId + "}";
            outError = null;

            Console.WriteLine($"回复请求: {outPayload}");

            // 返回 0 表示处理成功
            return 0;
        }
    }
}