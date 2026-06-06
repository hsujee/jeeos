/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 线程管理
 *
 * 实现线程相关系统调用：
 * - thread_create: 创建新线程
 * - thread_exit: 退出当前线程
 * - thread_join: 等待线程结束
 * - thread_yield: 让出CPU
 *
 * 函数接口：
 * - syscall_thread(): 系统调用表入口
 * - sys_thread_create(): 创建线程
 * - sys_thread_exit(): 退出线程
 *
 * 当前状态: 桩实现(待完善)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

// 系统调用表入口 - 桩实现
sysstus_t syscall_thread(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    (void)stkparv;
    return SYSSTUSERR;
}

// 创建线程接口 - 桩实现
hand_t sys_thread_create(void *entry, uint_t flags, void *arg) {
    (void)entry;
    (void)flags;
    (void)arg;
    return NO_HAND;
}

// 退出线程接口 - 桩实现
void sys_thread_exit(sysstus_t stus) {
    (void)stus;
    return;
}

// 等待线程接口 - 桩实现
sysstus_t sys_thread_wait(hand_t thhand) {
    (void)thhand;
    return SYSSTUSERR;
}

// 休眠接口 - 桩实现
sysstus_t sys_sleep(uint_t ms) {
    (void)ms;
    return SYSSTUSERR;
}
