/*
 * @Author: Jee Hsu
 * @Description: Fork/Wait/Exit 系统调用头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYSFORK_H
#define _SYSFORK_H

/**
 * @brief 进程退出
 * @param exit_code 退出码
 */
void sys_exit(sint_t exit_code);

/**
 * @brief 等待任意子进程退出
 * @param status 存储退出状态的指针
 * @return 退出的子进程 PID，无子进程返回 -1
 */
sint_t sys_wait(sint_t *status);

/**
 * @brief 等待指定子进程退出
 * @param pid 要等待的子进程 PID
 * @param status 存储退出状态的指针
 * @return 成功返回 PID，失败返回 -1
 */
sint_t sys_waitpid(uint_t pid, sint_t *status);

#endif // _SYSFORK_H

