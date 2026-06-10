/*
 * D1 SDK C 头文件 | 对应 D1 动态库版本: ≥ v1.1.0
 *
 * 本文件定义了 D1 动态库的完整 C 语言 API。
 * 使用前请确保已将 libd1.so (Linux) / libd1.dylib (macOS) / d1.dll (Windows)
 * 放置于正确的路径，并在编译时链接该动态库。
 *
 * 许可证: 参阅 D1 官方协议
 */

#ifndef D1_H
#define D1_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ──────────────────────────────────────────────────────────────────────────
 * 生命周期管理
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * D1_Version - 获取 D1 动态库的版本号字符串。
 *
 * 返回值:
 *   返回一个以 NUL 结尾的静态字符串，格式为 "major.minor.patch"。
 *   调用者无需释放该字符串。
 *   该函数可以在 D1_Init 之前调用。
 */
const char* D1_Version(void);

/**
 * D1_Init - 初始化 D1 运行时。
 *
 * 参数:
 *   config_path: 配置文件（JSON）的路径。传 NULL 表示使用默认配置。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败（具体错误码请参阅文档）。
 *
 * 注意:
 *   必须在调用任何其他非查询类函数之前调用此函数。
 *   该函数不是线程安全的，应在主线程中调用。
 */
int D1_Init(const char* config_path);

/**
 * D1_Start - 启动 D1 运行时，开始处理消息循环。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败。
 *
 * 注意:
 *   调用此函数后，D1 开始接收和处理消息。
 *   必须在 D1_Init 之后调用。
 */
int D1_Start(void);

/**
 * D1_Stop - 停止 D1 运行时，优雅关闭所有连接和任务。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败。
 *
 * 注意:
 *   调用此函数后，内部消息循环将退出。
 *   可以再次调用 D1_Start 重新启动。
 */
int D1_Stop(void);

/**
 * D1_WaitStop - 阻塞当前线程，直到收到退出信号，然后自动停止 D1。
 *
 * 推荐用法: Init() → Start() → WaitStop() → 进程退出
 *
 * 注意:
 *   - D1_WaitStop 内部自动监听 SIGINT/SIGTERM (Ctrl+C)
 *   - 收到信号后会自动调用 D1_Stop()，无需用户手动处理
 *   - 这是最简单标准的用法，推荐在大多数应用中使用
 */
void D1_WaitStop(void);

/* ──────────────────────────────────────────────────────────────────────────
 * 消息处理
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * D1_OnRequestFunc - 请求处理回调函数类型。
 *
 * 当 D1 收到一个需要本节点处理的请求消息时，会调用此回调。
 * 回调函数需要填充 out_payload / out_len 作为响应体，
 * 或填充 out_error 作为错误信息。
 *
 * 参数:
 *   task_id:     任务 ID，用于关联异步上下文。
 *   msg_name:    消息名称（以 NUL 结尾的字符串）。
 *   payload:     请求载荷（二进制安全数据）。
 *   payload_len: 载荷长度（字节数）。
 *   out_payload: 输出参数，指向响应载荷的指针。由回调分配内存，调用者通过 D1_Free 释放。
 *   out_len:     输出参数，响应载荷的长度。
 *   out_error:   输出参数，指向错误描述的指针。由回调分配内存，调用者通过 D1_Free 释放。
 *
 * 返回值:
 *   0 表示成功处理，非 0 表示处理失败。
 */
typedef int (*D1_OnRequestFunc)(uint64_t task_id,
                                const char* msg_name,
                                const char* payload,
                                int payload_len,
                                char** out_payload,
                                int* out_len,
                                char** out_error);

/**
 * D1_SetDefaultHandler - 设置默认的消息请求处理器。
 *
 * 参数:
 *   handler: D1_OnRequestFunc 类型的回调函数指针。
 *            传 NULL 可清除当前处理器。
 *
 * 注意:
 *   当收到的消息没有匹配到更具体的路由规则时，会回调该处理器。
 *   必须在 D1_Start 之前调用才能确保所有消息都能被处理。
 */
void D1_SetDefaultHandler(D1_OnRequestFunc handler);

/* ──────────────────────────────────────────────────────────────────────────
 * 异步响应回调
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * D1_OnResponse - 异步请求的响应回调函数类型。
 *
 * 当 D1_Call 或 D1_Request 的异步变体收到响应时，会调用此回调。
 *
 * 参数:
 *   task_id:      任务 ID，与发起请求时的 task_id 一致。
 *   payload:      响应载荷（二进制安全数据）。成功时非 NULL。
 *   payload_len:  响应载荷长度（字节数）。成功时 >= 0。
 *   error:        错误描述字符串。成功时为 NULL，失败时指向错误信息。
 */
typedef void (*D1_OnResponse)(uint64_t task_id,
                              const char* payload,
                              int payload_len,
                              const char* error);

/* ──────────────────────────────────────────────────────────────────────────
 * 消息发布与调用
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * D1_Publish - 发布（广播）一条消息到指定目标。
 *
 * 参数:
 *   task_id:      任务 ID（由调用者分配，用于追踪）。
 *   target:       目标标识符（如服务名、topic 等）。
 *   msg_name:     消息名称。
 *   payload:      消息载荷（二进制安全数据）。
 *   payload_len:  载荷长度（字节数），传 -1 表示 payload 为 NUL 结尾字符串。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败。
 *
 * 注意:
 *   该函数是异步的，不等待响应。消息会立即发送。
 */
int D1_Publish(uint64_t task_id,
               const char* target,
               const char* msg_name,
               const char* payload,
               int payload_len);

/**
 * D1_Call - 同步调用远程服务，阻塞等待响应。
 *
 * 参数:
 *   task_id:      任务 ID。
 *   kind:         处理器类型："default"(宿主程序回调), "conn"(外部第三方), "script"(内部脚本), "service"(微服务), "exec"(本地命令)，传 NULL 使用默认类型。
 *   target:       目标标识符。
 *   msg_name:     消息名称。
 *   payload:      请求载荷。
 *   payload_len:  载荷长度（字节数），传 -1 表示 NUL 结尾字符串。
 *   timeout_sec:  超时时间（秒）。0 表示无超时。
 *   out_payload:  输出参数，指向响应载荷的指针。调用者通过 D1_Free 释放。
 *   out_len:      输出参数，响应载荷的长度。
 *   out_error:    输出参数，指向错误信息的指针。调用者通过 D1_Free 释放。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败。
 */
int D1_Call(uint64_t task_id,
            const char* kind,
            const char* target,
            const char* msg_name,
            const char* payload,
            int payload_len,
            int timeout_sec,
            char** out_payload,
            int* out_len,
            char** out_error);

/**
 * D1_Request - 异步发送请求，通过回调接收响应。
 *
 * 参数:
 *   task_id:      任务 ID。
 *   target:       目标标识符。
 *   msg_name:     消息名称。
 *   payload:      请求载荷。
 *   payload_len:  载荷长度（字节数），传 -1 表示 NUL 结尾字符串。
 *   timeout_sec:  超时时间（秒）。
 *   callback:     响应回调函数。当收到响应或超时时被调用。
 *
 * 返回值:
 *   0 表示成功发送，非 0 表示失败。
 */
int D1_Request(uint64_t task_id,
               const char* target,
               const char* msg_name,
               const char* payload,
               int payload_len,
               int timeout_sec,
               D1_OnResponse callback);

/**
 * D1_Reply - 在处理请求的回调中发送响应。
 *
 * 参数:
 *   task_id:      任务 ID（必须与收到请求时的 task_id 一致）。
 *   msg_name:     消息名称。
 *   payload:      响应载荷。
 *   payload_len:  载荷长度（字节数），传 -1 表示 NUL 结尾字符串。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败。
 */
int D1_Reply(uint64_t task_id,
             const char* msg_name,
             const char* payload,
             int payload_len);

/* ──────────────────────────────────────────────────────────────────────────
 * 缓存操作
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * D1_CacheGet - 从 D1 内部缓存中读取键值。
 *
 * 参数:
 *   task_id:     任务 ID。
 *   key:         缓存键名（NUL 结尾字符串）。
 *   result:      输出参数，指向缓存值的指针。调用者通过 D1_Free 释放。
 *   result_len:  输出参数，缓存值的长度。未命中时设为 -1。
 *
 * 返回值:
 *   0 表示成功（含缓存未命中），非 0 表示失败。
 */
int D1_CacheGet(uint64_t task_id,
                const char* key,
                char** result,
                int* result_len);

/**
 * D1_CacheSet - 向 D1 内部缓存写入键值对。
 *
 * 参数:
 *   task_id:      任务 ID。
 *   key:          缓存键名。
 *   value:        缓存值（二进制安全数据）。
 *   value_len:    值的长度（字节数），传 -1 表示 NUL 结尾字符串。
 *   ttl_seconds:  生存时间（秒）。0 表示永不过期。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败。
 */
int D1_CacheSet(uint64_t task_id,
                const char* key,
                const char* value,
                int value_len,
                int ttl_seconds);

/**
 * D1_CacheDelete - 从 D1 内部缓存中删除指定键。
 *
 * 参数:
 *   task_id: 任务 ID。
 *   key:     缓存键名。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败。
 */
int D1_CacheDelete(uint64_t task_id, const char* key);

/* ──────────────────────────────────────────────────────────────────────────
 * 数据库操作
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * D1_DBQuery - 执行数据库查询语句（SELECT 等），返回结果集（JSON 格式）。
 *
 * 参数:
 *   task_id:     任务 ID。
 *   query:       查询 SQL 语句。
 *   query_len:   SQL 语句长度（字节数），传 -1 表示 NUL 结尾字符串。
 *   result:      输出参数，指向 JSON 格式结果集的指针。调用者通过 D1_Free 释放。
 *   result_len:  输出参数，结果集的长度。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败。
 */
int D1_DBQuery(uint64_t task_id,
               const char* query,
               int query_len,
               char** result,
               int* result_len);

/**
 * D1_DBExec - 执行数据库变更语句（INSERT/UPDATE/DELETE 等）。
 *
 * 参数:
 *   task_id:       任务 ID。
 *   query:         变更 SQL 语句。
 *   query_len:     SQL 语句长度（字节数），传 -1 表示 NUL 结尾字符串。
 *   affected_rows: 输出参数，受影响的行数。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败。
 */
int D1_DBExec(uint64_t task_id,
              const char* query,
              int query_len,
              int64_t* affected_rows);

/* ──────────────────────────────────────────────────────────────────────────
 * 临时键值存储
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * D1_Set - 设置临时键值对（内存存储，进程重启后丢失）。
 *
 * 参数:
 *   task_id:    任务 ID。
 *   key:        键名。
 *   value:      键值（二进制安全数据）。
 *   value_len:  值的长度（字节数），传 -1 表示 NUL 结尾字符串。
 *
 * 返回值:
 *   0 表示成功，非 0 表示失败。
 */
int D1_Set(uint64_t task_id,
           const char* key,
           const char* value,
           int value_len);

/**
 * D1_Get - 获取临时键值对。
 *
 * 参数:
 *   task_id:    任务 ID。
 *   key:        键名。
 *   result:     输出参数，指向键值的指针。调用者通过 D1_Free 释放。
 *   result_len: 输出参数，键值的长度。未找到时设为 -1。
 *
 * 返回值:
 *   0 表示成功（含键未找到），非 0 表示失败。
 */
int D1_Get(uint64_t task_id,
           const char* key,
           char** result,
           int* result_len);

/* ──────────────────────────────────────────────────────────────────────────
 * 内存管理
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * D1_Free - 释放由 D1 API 内部分配的内存。
 *
 * 参数:
 *   ptr: 指向需要释放的内存的指针。传 NULL 无操作。
 *
 * 注意:
 *   所有通过 out_payload / out_error / result 等输出参数返回的内存，
 *   都必须通过此函数释放，不能使用标准库的 free()。
 */
void D1_Free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif /* D1_H */