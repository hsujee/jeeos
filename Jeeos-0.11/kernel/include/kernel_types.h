/*
 * @Author: Jee Hsu
 * @Description: 内核类型定义聚合
 *
 * 包含所有内核层相关的类型定义头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _KERNEL_TYPES_H
#define _KERNEL_TYPES_H

/* 中断和内存类型 */
#include "interrupt_t.h"
#include "mm_pages_t.h"
#include "mm_t.h"
#include "vmem_t.h"

/* I/O和设备类型 */
#include "io_node_t.h"
#include "device_t.h"
#include "iocache_t.h"

/* 时间和全局类型 */
#include "time_t.h"
#include "global_t.h"

/* 线程和调度类型 */
#include "thread_t.h"
#include "sched_t.h"
#include "wait_list_t.h"
#include "cpu_idle_t.h"

#endif // _KERNEL_TYPES_H

