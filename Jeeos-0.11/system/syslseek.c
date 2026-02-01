/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 文件定位(lseek)
 *
 * 实现POSIX lseek()系统调用：
 * - 移动文件读写指针
 * - 支持SEEK_SET/SEEK_CUR/SEEK_END
 * - 返回新的文件位置
 *
 * 函数接口：
 * - syscall_lseek(): 系统调用表入口
 * - sys_lseek(): 用户接口
 *
 * 当前状态: 桩实现(待完善)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

// 系统调用表入口 - 桩实现
sysstus_t syscall_lseek(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    (void)stkparv;
    return SYSSTUSERR;
}

// 系统定位接口 - 桩实现
sysstus_t sys_lseek(hand_t fhand, uint_t pos) {
    (void)fhand;
    (void)pos;
    return SYSSTUSERR;
}
