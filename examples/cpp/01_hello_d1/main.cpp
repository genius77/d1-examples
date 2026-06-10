/*
 * D1 C++ 示例 —— 01_hello_d1
 *
 * 本示例演示 D1 C++ 封装类 (D1) 的最基本用法:
 *   1. 获取版本号
 *   2. 初始化
 *   3. 启动
 *   4. 设置消息处理器（使用 C++ lambda 表达式）
 *   5. 等待退出信号
 *   6. 停止并等待清理
 *
 * 使用前请将 libd1.so 放置于 deps/ 目录。
 */

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

/* 引入 D1 C++ 封装头文件 */
#include "../../../lang/cpp/d1.hpp"

/* ──────────────────────────────────────────────────────────────────────────
 * 全局状态
 * ────────────────────────────────────────────────────────────────────────── */

/* 使用 std::atomic 保证线程安全 */
static std::atomic<bool> g_running{true};

/* ──────────────────────────────────────────────────────────────────────────
 * 信号处理
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * on_signal - 处理 SIGINT (Ctrl+C) 和 SIGTERM 信号。
 */
static void on_signal(int sig) {
    (void)sig;
    std::cerr << "\n[信号] 收到退出信号，正在优雅关闭..." << std::endl;
    g_running.store(false);
}

/* ──────────────────────────────────────────────────────────────────────────
 * 辅助函数：错误检查宏
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * check_or_exit - 如果调用抛出异常则捕获并退出。
 *
 * 用于包装可能抛出 std::runtime_error 的 D1 方法调用。
 */
template <typename F>
static void check_or_exit(const std::string& label, F&& fn) {
    try {
        fn();
        std::cout << "[信息] " << label << " 成功" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[错误] " << label << " 失败: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/* ──────────────────────────────────────────────────────────────────────────
 * 主函数
 * ────────────────────────────────────────────────────────────────────────── */

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    /* ── 1. 获取版本号 ── */
    std::string version = D1::Version();
    if (version.empty()) {
        std::cerr << "[错误] D1::Version() 返回空字符串，"
                  << "请检查动态库是否存在。" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[信息] D1 版本: " << version << std::endl;

    /* ── 2. 初始化 D1 运行时 ── */
    /* 传 nullptr 使用默认配置 */
    check_or_exit("D1::Init()", [] { D1::Init(nullptr); });

    /* ── 3. 设置默认消息处理器 —— 使用 C++ lambda ── */
    D1::SetDefaultHandler(
        [](uint64_t task_id, const char* msg_name, const char* payload,
           int payload_len, char** out_payload, int* out_len,
           char** out_error) -> int {
            (void)payload;
            (void)payload_len;

            std::cout << "[处理器] task_id=" << task_id
                      << ", msg_name=" << (msg_name ? msg_name : "(null)")
                      << std::endl;

            /* 使用 std::string 构造响应 */
            std::string reply =
                R"({"status":"ok","msg":"hello from C++ lambda handler"})";

            *out_payload = static_cast<char*>(
                std::malloc(reply.size()));
            if (!*out_payload) {
                *out_error = strdup("malloc 失败");
                return -1;
            }
            std::memcpy(*out_payload, reply.data(), reply.size());
            *out_len   = static_cast<int>(reply.size());
            *out_error = nullptr;

            return 0;
        });
    std::cout << "[信息] 已设置默认消息处理器 (C++ lambda)" << std::endl;

    /* ── 4. 注册信号处理 ── */
    std::signal(SIGINT, on_signal);
    std::signal(SIGTERM, on_signal);
    std::cout << "[信息] 已注册 SIGINT / SIGTERM 信号处理" << std::endl;

    /* ── 5. 启动 D1 运行时 ── */
    check_or_exit("D1::Start()", [] { D1::Start(); });
    std::cout << "[信息] D1 运行时已启动" << std::endl;

    /* ── 6. 主循环：等待退出信号 ── */
    std::cout << "[信息] D1 运行中，按 Ctrl+C 退出..." << std::endl;
    while (g_running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    /* ── 7. 停止 D1 运行时 ── */
    std::cout << "[信息] 正在停止 D1..." << std::endl;
    check_or_exit("D1::Stop()", [] { D1::Stop(); });

    /* ── 8. 等待清理完成 ── */
    D1::Wait();
    std::cout << "[信息] D1 已完全停止，程序退出。" << std::endl;

    return EXIT_SUCCESS;
}