// D1 C# 入门示例: Hello D1
// 演示 D1 SDK 基本生命周期:
//   1. 打印版本号
//   2. 初始化 D1 (使用默认配置)
//   3. 设置默认请求处理器
//   4. 启动 D1 运行时
//   5. 阻塞等待退出（WaitStop 内置信号处理 + 自动停止）
//
// D1 动态库依赖: >= v1.2.0
//
// 运行前提: 确保 D1 动态库已放入 deps/ 目录。

using System;
using Genius77.D1;

namespace D1HelloWorld
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("=== D1 C# Hello D1 ===");

            try
            {
                // 1. 获取并打印 D1 动态库版本
                string version = D1.Version();
                Console.WriteLine($"D1 version: {version}");

                // 2. 使用默认配置初始化 D1
                //    传入 null 表示从 deps/ 读取默认配置
                D1.Init(null);
                Console.WriteLine("D1::Init() succeeded");

                // 3. 设置默认请求处理器
                //    所有未被路由匹配的请求都会回调此处理器
                D1.SetDefaultHandler(OnRequest);
                Console.WriteLine("Default handler registered");

                // 4. 启动 D1 运行时
                D1.Start();
                Console.WriteLine("D1::Start() succeeded");

                // 5. 阻塞等待退出，自动停止
                //    D1::WaitStop() 内部监听 SIGINT/SIGTERM (Ctrl+C)，
                //    收到信号后自动调用 D1::Stop() 完成优雅退出
                Console.WriteLine("D1 running, press Ctrl+C to exit...");
                D1.WaitStop();
                Console.WriteLine("D1 fully stopped, exiting.");
            }
            catch (D1Exception ex)
            {
                Console.WriteLine($"D1 error: {ex.Message} (code={ex.ErrorCode}, fn={ex.FunctionName})");
                Environment.Exit(1);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Unexpected error: {ex.Message}");
                Environment.Exit(1);
            }
        }

        /// <summary>
        /// 默认请求处理回调。
        /// 记录请求信息并返回简单的响应。
        /// </summary>
        static int OnRequest(ulong taskId, string msgName, string payload,
            out string outPayload, out string outError)
        {
            Console.WriteLine($"[Handler] taskId={taskId}, msgName={msgName}, payload={payload}");

            outPayload = $"{{\"status\":\"ok\",\"msg\":\"hello from C# handler\",\"taskId\":{taskId}}}";
            outError = null;

            return 0;
        }
    }
}