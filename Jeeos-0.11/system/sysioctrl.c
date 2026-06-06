/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 设备IO控制(ioctl)
 *
 * 实现POSIX ioctl()系统调用：
 * - 设备特定操作(非标准I/O)
 * - 配置设备参数
 * - 查询设备状态
 *
 * 常用命令：
 * - 终端控制(TIOC*)
 * - 串口配置
 * - 网络接口配置
 *
 * 函数接口：
 * - syscall_ioctrl(): 系统调用表入口
 * - sys_ioctrl(): 用户接口
 *
 * 当前状态: 桩实现(待完善)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

/**
 * 系统调用表入口 - 桩实现
 */
sysstus_t syscall_ioctrl(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    (void)stkparv;
    return SYSSTUSERR;
}

/**
 * 系统IO控制接口 - 桩实现
 */
sysstus_t sys_ioctrl(hand_t fhand, uint_t ctrlcmd, void *buf, size_t bufsz) {
    (void)fhand;
    (void)ctrlcmd;
    (void)buf;
    (void)bufsz;
    return SYSSTUSERR;
}

/**
 * 核心IO控制实现 - 桩实现
 */
sysstus_t sys_core_ioctrl(hand_t fhand, uint_t ctrlcmd, void *buf, size_t bufsz) {
    (void)fhand;
    (void)ctrlcmd;
    (void)buf;
    (void)bufsz;
    return SYSSTUSERR;
}

/**
 * 设备IO控制 - 桩实现
 */
sysstus_t sys_ioctrl_device(objnode_t *ondep) {
    (void)ondep;
    return SYSSTUSERR;
}
