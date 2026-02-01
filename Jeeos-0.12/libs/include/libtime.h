/*
 * @Author: Jee Hsu
 * @Description: 用户态库 - 时间管理接口声明
 *
 * 提供用户态时间操作接口：
 * - time: 获取系统时间
 * - settime/gettime: 设置/获取RTC时间
 * - synsecalarm: 秒级同步闹钟
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _LIBTIME_H
#define _LIBTIME_H

#include "libtypes.h"

/* 时间获取 */
sysstus_t time(times_t* ttime);
sysstus_t settime(times_t* ttime);
sysstus_t gettime(times_t* ttime);
sysstus_t synsecalarm(uint_t sec);

/* 延时函数 */
uint_t sleep(uint_t seconds);       /**< 秒级睡眠 */
int usleep(uint_t usec);            /**< 微秒级睡眠 */
int msleep(uint_t msec);            /**< 毫秒级睡眠 */

#endif /* _LIBTIME_H */
