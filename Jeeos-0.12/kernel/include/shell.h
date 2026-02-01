/*
 * @Author: Jee Hsu
 * @Description: Jeeos Shell 命令行解释器
 *
 * 提供基本的命令行交互界面：
 * - 键盘输入处理
 * - 命令解析和执行
 * - 内置命令支持
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SHELL_H
#define _SHELL_H

#include "btypes.h"

/* Shell 配置 */
#define SHELL_CMDLINE_MAX   256     /* 命令行最大长度 */
#define SHELL_ARGS_MAX      16      /* 最大参数数量 */
#define SHELL_PROMPT        "jeeos> "

/**
 * @brief 初始化并启动 Shell
 * 
 * 创建 Shell 内核线程
 */
void init_shell(void);

/**
 * @brief Shell 主循环
 * 
 * 内核线程入口函数
 */
void shell_main(void);

/**
 * @brief 执行命令
 * @param cmdline 命令行字符串
 * @return 0成功，非0失败
 */
int shell_execute(char *cmdline);

#endif /* _SHELL_H */

