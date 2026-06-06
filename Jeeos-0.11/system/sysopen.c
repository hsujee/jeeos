/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 文件打开(open)
 *
 * 实现POSIX open()系统调用：
 * - 解析文件路径
 * - 查找/创建文件
 * - 分配文件句柄
 * - 返回句柄给用户态
 *
 * 函数接口：
 * - syscall_open(): 系统调用表入口
 * - sys_open(): 用户接口
 * - sys_core_open(): 核心实现
 *
 * 当前状态: 桩实现(待完善)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

// 系统调用表入口 - 桩实现
sysstus_t syscall_open(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    (void)stkparv;
    return SYSSTUSERR;
}

// 系统打开接口 - 桩实现
hand_t sys_open(void *file, uint_t flags, uint_t stus) {
    (void)file;
    (void)flags;
    (void)stus;
    return NO_HAND;
}

// 核心打开实现 - 桩实现
hand_t sys_core_open(void *file, uint_t flags, uint_t stus) {
    (void)file;
    (void)flags;
    (void)stus;
    return NO_HAND;
}

// 设备打开 - 桩实现
sysstus_t sys_open_device(objnode_t *ondep) {
    (void)ondep;
    return SYSSTUSERR;
}
