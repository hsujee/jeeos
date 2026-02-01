/*
 * @Author: Jee Hsu
 * @Description: Init 进程头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _INIT_H
#define _INIT_H

/**
 * @brief 创建 init 进程
 */
void init_process(void);

/**
 * @brief 获取 init 进程指针
 */
thread_t *get_init_thread(void);

/**
 * @brief Init 进程主函数
 */
void init_main(void);

#endif // _INIT_H

