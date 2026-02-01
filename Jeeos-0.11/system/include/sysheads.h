/*
 * @Author: Jee Hsu
 * @Description: 系统服务层头文件聚合
 *
 * 包含所有系统调用相关的头文件（桩实现）
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYSHEADS_H
#define _SYSHEADS_H

/* 系统调用处理 */
#include "syshandler.h"

/* 内存管理 */
#include "sysmm.h"

/* 线程管理 */
#include "systhread.h"

/* 文件操作 */
#include "sysopen.h"
#include "sysclose.h"
#include "sysread.h"
#include "syswrite.h"
#include "sysioctrl.h"
#include "syslseek.h"

#endif // _SYSHEADS_H

