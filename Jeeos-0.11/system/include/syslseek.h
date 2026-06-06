/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - lseek文件定位
 *
 * 提供文件定位的系统调用接口（桩实现）
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_LSEEK_H
#define _SYS_LSEEK_H

/**
 * @brief 系统调用表lseek入口
 * @param inr 系统调用号
 * @param stkparv 栈参数
 * @return 系统调用状态码
 */
sysstus_t syscall_lseek(uint_t inr, stkparame_t* stkparv);

/**
 * @brief 文件定位
 * @param fhand 文件句柄
 * @param pos 目标位置
 * @return 系统调用状态码
 */
sysstus_t sys_lseek(hand_t fhand, uint_t pos);

#endif // _SYS_LSEEK_H
