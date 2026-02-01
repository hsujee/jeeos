/*
 * @Author: Jee Hsu
 * @Description: 基础类型定义聚合
 *
 * 包含系统通用的基础类型定义：
 * - typedef.h: 基本数据类型(u8_t, u16_t, u32_t, u64_t等)
 * - list_t.h: 链表数据结构
 * - spin_lock_t.h: 自旋锁类型
 * - atomic_t.h: 原子操作类型
 * - sem_t.h: 信号量类型
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _BTYPES_H
#define _BTYPES_H

/* 基础类型 */
#include "typedef.h"

/* 数据结构类型 */
#include "list_t.h"
#include "spin_lock_t.h"
#include "atomic_t.h"
#include "sem_t.h"
#include "mutex_t.h"

/* 操作函数 */
#include "atomic.h"
#include "list.h"
#include "mutex.h"

#endif /* _BTYPES_H */
