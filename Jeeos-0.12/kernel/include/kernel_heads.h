/*
 * @Author: Jee Hsu
 * @Description: 内核头文件聚合
 *
 * 包含所有内核层相关的头文件，方便统一引用
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _KERNEL_HEADS_H
#define _KERNEL_HEADS_H

/* 全局定义和同步原语 */
#include "global.h"
#include "kernel_error.h"
#include "spin_lock.h"
#include "sem.h"
#include "wait_list.h"

/* 线程和调度 */
#include "thread.h"
#include "sched.h"

/* 内存管理 */
#include "mm.h"
#include "mm_pages.h"
#include "vmem.h"

/* 设备管理 */
#include "device.h"
#include "driver_helper.h"
#include "io_node.h"
#include "iocache.h"

/* 时间和空闲处理 */
#include "time.h"
#include "cpu_idle.h"

/* 中断和入口 */
#include "interrupt.h"
#include "main.h"

/* Shell */
#include "shell.h"

/* Init 进程 */
#include "init.h"

/* 调试工具 */
#include "debug_uart.h"

#endif // _KERNEL_HEADS_H

