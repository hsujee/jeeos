/*
 * @Author: Jee Hsu
 * @Description: 互斥锁类型定义
 *
 * 提供用户态互斥锁数据结构：
 * - mutex_t: 互斥锁
 * - 支持递归锁和超时等待
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _MUTEX_T_H
#define _MUTEX_T_H

#include "atomic_t.h"

/** @brief 互斥锁状态 */
#define MUTEX_UNLOCKED      0
#define MUTEX_LOCKED        1

/**
 * @brief 互斥锁结构
 * 
 * 提供线程间互斥访问共享资源
 */
typedef struct s_MUTEX {
    atomic_t state;             /**< 锁状态: UNLOCKED/LOCKED */
    volatile s64_t owner;       /**< 持有者线程ID (-1表示无) */
    volatile u32_t count;       /**< 递归计数 */
} mutex_t;

/** @brief 互斥锁静态初始化 */
#define MUTEX_INIT  { ATOMIC_INIT(MUTEX_UNLOCKED), -1, 0 }

/**
 * @brief 条件变量结构
 * 
 * 用于线程间同步等待
 */
typedef struct s_CONDVAR {
    atomic_t waiters;           /**< 等待者计数 */
    atomic_t signal;            /**< 信号标志 */
} condvar_t;

/** @brief 条件变量静态初始化 */
#define CONDVAR_INIT    { ATOMIC_INIT(0), ATOMIC_INIT(0) }

#endif /* _MUTEX_T_H */

