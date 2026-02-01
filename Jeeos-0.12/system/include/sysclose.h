/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - close关闭操作
 *
 * 提供文件/设备关闭的系统调用接口（桩实现）
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_CLOSE_H
#define _SYS_CLOSE_H

#include "typedef.h"       /* sysstus_t, hand_t 等基础类型 */
#include "syshandler_t.h"  /* stkparame_t */

struct s_OBJNODE;
typedef struct s_OBJNODE objnode_t;

/**
 * @brief 系统调用表close入口
 * @param inr 系统调用号
 * @param stkparv 栈参数
 * @return 系统调用状态码
 */
sysstus_t syscall_close(uint_t inr, stkparame_t* stkparv);

/**
 * @brief 关闭文件/设备
 * @param fhand 文件/设备句柄
 * @return 系统调用状态码
 */
sysstus_t sys_close(hand_t fhand);

/**
 * @brief 关闭文件/设备核心函数
 * @param fhand 文件/设备句柄
 * @return 系统调用状态码
 */
sysstus_t sys_core_close(hand_t fhand);

/**
 * @brief 设备关闭操作
 * @param ondep 对象节点指针
 * @return 系统调用状态码
 */
sysstus_t sys_close_device(objnode_t* ondep);

#endif // _SYS_CLOSE_H
