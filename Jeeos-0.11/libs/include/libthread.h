/*
 * @Author: Jee Hsu
 * @Description: 用户态库 - 线程管理接口声明
 *
 * 提供用户态线程操作接口：
 * - exel: 执行/创建线程
 * - exit: 退出当前线程
 * - pid: 获取线程句柄
 * - retn_threadstats/set_threadstats: 获取/设置线程状态
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _LIBTHREAD_H
#define _LIBTHREAD_H

#include "libtypes.h"

/* 线程操作 */
hand_t    exel(void* file, uint_t flags);
void      exit(void);
hand_t    pid(void* tname);
sysstus_t retn_threadstats(hand_t thand, uint_t scode, uint_t data, buf_t buf);
sysstus_t set_threadstats(hand_t thand, uint_t scode, uint_t data, buf_t buf);

#endif /* _LIBTHREAD_H */

