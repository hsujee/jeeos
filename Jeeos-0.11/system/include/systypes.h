/*
 * @Author: Jee Hsu
 * @Description: 系统服务层类型定义聚合
 *
 * 包含所有系统调用相关的类型定义头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYSTYPES_H
#define _SYSTYPES_H

/* 系统调用处理类型 */
#include "syshandler_t.h"

/* 内存管理类型 */
#include "sysmm_t.h"

/* 线程管理类型 */
#include "systhread_t.h"

/* 文件操作类型 */
#include "sysopen_t.h"
#include "sysclose_t.h"
#include "sysread_t.h"
#include "syswrite_t.h"
#include "sysioctrl_t.h"
#include "syslseek_t.h"

#endif // _SYSTYPES_H

