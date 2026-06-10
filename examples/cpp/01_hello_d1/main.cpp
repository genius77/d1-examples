/*
 * D1 C++ 示例 —— 01_hello_d1
 *
 * 本示例演示 D1 C++ 封装类 (D1) 的最基本用法:
 *   1. 获取版本号
 *   2. 初始化
 *   3. 设置消息处理器（使用 C++ lambda 表达式）
 *   4. 启动
 *   5. 阻塞等待退出（WaitStop 内置信号处理 + 自动停止）
 *
 * D1 动态库依赖: ≥ v1.2.0
 *
 * 使用前请将 libd1.so / libd1.dylib / d1.dll 放入 deps/ 目录。
 */

#include <iostream>
#include <string>

/* 引入 D1 C++ 封装头文件 */
#include "../../../lang/cpp/d1.hpp"

/* ──────────────────────────────────────────────────────────────────────────
 * 主函数
 * ────────────────────────────────────────────────────────────────────────── */

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    /* ── 1. 获取版本号 ── */
    std::string version = D1::Version();
    if (version.empty()) {
        std::cerr << "[Error] D1::Version() returned empty string, "
                  << "please check that the dynamic library exists." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[Info] D1 version: " << version << std::endl;

    /* ── 2. 初始化 D1 运行时 ── */
    /* nullptr = use default config (reads from deps/ directory) */
    try {
        D1::Init(nullptr);
        std::cout << "[Info] D1::Init() succeeded" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Error] D1::Init() failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    /* ── 3. 设置默认消息处理器（使用 C++ lambda） ── */
    D1::SetDefaultHandler(
        [](uint64_t task_id, const char* msg_name, const char* payload,
           int payload_len, char** out_payload, int* out_len,
           char** out_error) -> int {
            (void)payload;
            (void)payload_len;

            std::cout << "[Handler] task_id=" << task_id
                      << ", msg_name=" << (msg_name ? msg_name : "(null)")
                      << std::endl;

            std::string reply =
                R"({"status":"ok","msg":"hello from C++ lambda handler"})";

            *out_payload = static_cast<char*>(std::malloc(reply.size()));
            if (!*out_payload) {
                *out_error = strdup("malloc failed");
                return -1;
            }
            std::memcpy(*out_payload, reply.data(), reply.size());
            *out_len   = static_cast<int>(reply.size());
            *out_error = nullptr;

            return 0;
        });
    std::cout << "[Info] Default handler set (C++ lambda)" << std::endl;

    /* ── 4. 启动 D1 运行时 ── */
    try {
        D1::Start();
        std::cout << "[Info] D1::Start() succeeded" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Error] D1::Start() failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    /* ── 5. 阻塞等待退出，自动停止 ── */
    /* D1::WaitStop() internally listens for SIGINT/SIGTERM (Ctrl+C),
       and automatically calls D1::Stop() for graceful shutdown. */
    std::cout << "[Info] D1 running, press Ctrl+C to exit..." << std::endl;
    D1::WaitStop();
    std::cout << "[Info] D1 fully stopped, exiting." << std::endl;

    return EXIT_SUCCESS;
}