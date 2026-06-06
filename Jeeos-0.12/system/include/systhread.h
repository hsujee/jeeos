/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 线程管理
 *
 * 提供线程管理的系统调用接口（桩实现）
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_THREAD_H
#define _SYS_THREAD_H

#include "typedef.h"       /* sysstus_t, uint_t, hand_t 等基础类型 */
#include "syshandler_t.h"  /* stkparame_t */

/**
 * @brief 系统调用表thread入口
 * @param inr 系统调用号
 * @param stkparv 栈参数
 * @return 系统调用状态码
 */
sysstus_t syscall_thread(uint_t inr, stkparame_t* stkparv);

/**
 * @brief 创建线程
 * @param entry 线程入口函数
 * @param flgs 创建标志
 * @param arg 线程参数
 * @return 线程句柄，失败返回NO_HAND
 */
hand_t sys_thread_create(void *entry, uint_t flags, void *arg);

/**
 * @brief 退出线程
 * @param stus 退出状态
 */
void sys_thread_exit(sysstus_t stus);

/**
 * @brief 等待线程
 * @param thhand 线程句柄
 * @return 系统调用状态码
 */
sysstus_t sys_thread_wait(hand_t thhand);

/**
 * @brief 休眠
 * @param ms 休眠毫秒数
 * @return 系统调用状态码
 */
sysstus_t sys_sleep(uint_t ms);

#endif // _SYS_THREAD_H
