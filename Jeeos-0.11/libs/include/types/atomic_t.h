/*
 * @Author: Jee Hsu
 * @Description: 原子操作类型定义
 *
 * 提供原子操作的数据类型：
 * - atomic_t: 原子整数(用于计数器、标志等)
 * - refcount_t: 引用计数(用于对象生命周期管理)
 *
 * 原子操作保证在多核环境下的数据一致性
 * 无需使用锁即可安全访问
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _ATOMIC_T_H
#define _ATOMIC_T_H

/**
 * @brief 32位原子整数结构
 * 
 * 用于计数器、标志等需要原子操作的场景
 * 通过 atomic_add/sub/inc/dec 等函数操作
 */
typedef struct s_ATOMIC {
    volatile s32_t a_count;     /**< 原子计数值 */
} atomic_t;

/**
 * @brief 64位原子整数结构
 * 
 * 用于需要更大范围的原子计数场景
 * 在64位系统上保证原子性
 */
typedef struct s_ATOMIC64 {
    volatile s64_t a_count;     /**< 64位原子计数值 */
} atomic64_t;

/**
 * @brief 引用计数结构
 * 
 * 用于对象生命周期管理
 * 当引用计数降为0时释放对象
 */
typedef struct s_REFCOUNT {
    atomic_t ref_count;         /**< 引用计数(原子操作) */
} refcount_t;

/** @brief 原子变量静态初始化 */
#define ATOMIC_INIT(i)      { (i) }
#define ATOMIC64_INIT(i)    { (i) }

#endif
