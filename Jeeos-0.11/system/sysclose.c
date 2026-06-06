/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 文件关闭(close)
 *
 * 实现POSIX close()系统调用：
 * - 验证文件句柄
 * - 刷新缓冲区
 * - 释放文件句柄
 * - 减少引用计数
 *
 * 函数接口：
 * - syscall_close(): 系统调用表入口
 * - sys_close(): 用户接口
 * - sys_core_close(): 核心实现
 *
 * 当前状态: 桩实现(待完善)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

// 系统调用表入口 - 桩实现
sysstus_t syscall_close(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    (void)stkparv;
    return SYSSTUSERR;
}

// 系统关闭接口 - 桩实现
sysstus_t sys_close(hand_t fhand) {
    (void)fhand;
    return SYSSTUSERR;
}

// 核心关闭实现 - 桩实现
sysstus_t sys_core_close(hand_t fhand) {
    (void)fhand;
    return SYSSTUSERR;
}

// 设备关闭 - 桩实现
sysstus_t sys_close_device(objnode_t *ondep) {
    (void)ondep;
    return SYSSTUSERR;
}
