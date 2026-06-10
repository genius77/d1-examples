#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
示例: D1 Hello World (Python)

本示例演示 D1 SDK 的基础使用流程:
  1. 获取版本号
  2. 初始化 D1 运行时
  3. 设置默认消息处理器
  4. 启动 D1
  5. 阻塞等待退出（WaitStop 内置信号处理）

使用前请确保已将 libd1.so / libd1.dylib / d1.dll 和 d1.h 放入 deps/ 目录。

运行方式:
    cd examples/python/01_hello_d1
    python3 main.py

也可以通过 D1_CONFIG 环境变量指定配置文件路径:
    D1_CONFIG=/path/to/config.yaml python3 main.py
"""

import os
import sys

# 将 SDK 目录加入 Python 路径
# 路径说明: examples/python/01_hello_d1/main.py -> ../../lang/python/
_sdk_path = os.path.normpath(
    os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "sdk", "python")
)
sys.path.insert(0, _sdk_path)

from d1 import D1


def default_handler(task_id, msg_name, payload):
    """默认消息处理器。

    当 D1 收到未匹配到特定路由的消息时，将调用此函数。

    Args:
        task_id: 任务标识 (int)
        msg_name: 消息名称 (str)
        payload: 消息载荷 (bytes)

    Returns:
        bytes: 响应载荷，或 None 表示无响应。
    """
    # 将载荷解码为字符串便于显示
    payload_str = payload.decode("utf-8") if payload else ""
    print(f"[处理器] 收到消息 | taskID={task_id} | msgName={msg_name} | payload={payload_str}")

    # 返回回显响应
    return f'{{"echo":"{payload_str}"}}'.encode("utf-8")


def main():
    """主函数: D1 Hello World 示例。"""
    print("===== D1 Hello World (Python) =====")

    # 使用上下文管理器自动管理 D1 生命周期
    with D1() as d1:
        # 1. 输出版本信息
        ver = d1.version()
        print(f"D1 动态库版本: {ver}")

        # 2. 初始化 D1 运行时
        #    传 None 表示使用内置默认配置
        #    也可以传入配置文件路径，如: d1.init("config.yaml")
        config_path = os.environ.get("D1_CONFIG", None)
        config_info = config_path if config_path else "默认配置"
        print(f"正在初始化 D1 (配置: {config_info})...")
        d1.init(config_path)
        print("D1 初始化成功")

        # 3. 设置默认消息处理器
        d1.set_default_handler(default_handler)
        print("默认消息处理器已设置")

        # 4. 启动 D1 运行时
        print("正在启动 D1...")
        d1.start()
        print("D1 启动成功，正在运行... (按 Ctrl+C 退出)")

        # 5. 阻塞等待退出
        #    D1_WaitStop 内部监听 SIGINT/SIGTERM，
        #    收到 Ctrl+C 后自动调用 stop()，无需手动信号处理
        d1.wait_stop()

    print("D1 已正常退出，示例结束")


if __name__ == "__main__":
    main()