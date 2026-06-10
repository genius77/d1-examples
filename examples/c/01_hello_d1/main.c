/*
 * D1 C 语言示例 —— 01_hello_d1
 *
 * 本示例演示 D1 C API 的最基本用法:
 *   1. 获取版本号
 *   2. 初始化
 *   3. 设置消息处理器
 *   4. 启动
 *   5. 阻塞等待退出（WaitStop 内置信号处理）
 *
 * 编译时需要链接 libd1.so，详见同目录下的 CMakeLists.txt。
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 引入 D1 SDK 头文件 */
#include "../../../lang/c/d1.h"

/* ──────────────────────────────────────────────────────────────────────────
 * D1 请求处理器
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * on_request - 默认消息请求处理回调。
 *
 * 当 D1 收到未被路由到特定处理器的消息时，会调用此函数。
 * 这里简单返回一个确认响应。
 */
static int on_request(uint64_t task_id,
                      const char* msg_name,
                      const char* payload,
                      int payload_len,
                      char** out_payload,
                      int* out_len,
                      char** out_error) {
    (void)payload;
    (void)payload_len;

    printf("[处理器] task_id=%lu, msg_name=%s\n",
           (unsigned long)task_id,
           msg_name ? msg_name : "(null)");

    /* 构造一个简单的响应 */
    const char* reply = "{\"status\":\"ok\",\"msg\":\"hello from C handler\"}";
    int reply_len = (int)strlen(reply);

    /* 使用 malloc 分配响应内存，D1 内部会通过 D1_Free 释放 */
    *out_payload = (char*)malloc((size_t)reply_len);
    if (!*out_payload) {
        *out_error = strdup("malloc 失败");
        return -1;
    }
    memcpy(*out_payload, reply, (size_t)reply_len);
    *out_len   = reply_len;
    *out_error = NULL;

    return 0; /* 0 表示处理成功 */
}

/* ──────────────────────────────────────────────────────────────────────────
 * 主函数
 * ────────────────────────────────────────────────────────────────────────── */

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    int ret = 0;

    /* ── 1. 获取版本号 ── */
    const char* version = D1_Version();
    if (!version) {
        fprintf(stderr, "[错误] D1_Version() 返回 NULL，请检查动态库是否存在。\n");
        return EXIT_FAILURE;
    }
    printf("[信息] D1 版本: %s\n", version);

    /* ── 2. 初始化 D1 运行时 ── */
    /* 传 NULL 表示使用默认配置 */
    ret = D1_Init(NULL);
    if (ret != 0) {
        fprintf(stderr, "[错误] D1_Init() 失败，返回值: %d\n", ret);
        return EXIT_FAILURE;
    }
    printf("[信息] D1_Init() 成功\n");

    /* ── 3. 设置默认消息处理器（必须在 Start 前设置） ── */
    D1_SetDefaultHandler(on_request);
    printf("[信息] 已设置默认消息处理器\n");

    /* ── 4. 启动 D1 运行时 ── */
    ret = D1_Start();
    if (ret != 0) {
        fprintf(stderr, "[错误] D1_Start() 失败，返回值: %d\n", ret);
        return EXIT_FAILURE;
    }
    printf("[信息] D1_Start() 成功，D1 运行时已启动\n");

    /* ── 5. 阻塞等待退出信号，自动停止 ── */
    /* D1_WaitStop 内部监听 SIGINT/SIGTERM (Ctrl+C)，
       收到信号后自动调用 D1_Stop() 完成优雅退出 */
    printf("[信息] D1 运行中，按 Ctrl+C 退出...\n");
    D1_WaitStop();
    printf("[信息] D1 已完全停止，程序退出。\n");

    return EXIT_SUCCESS;
}