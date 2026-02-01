/*
 * @Author: Jee Hsu
 * @Description: 自旋锁类型定义
 *
 * 自旋锁是一种忙等待的同步原语：
 * - 适用于短临界区保护
 * - 持锁期间不能睡眠
 * - 多核环境下保护共享数据
 *
 * 使用方法：
 * spinlock_init(&lock);      // 初始化
 * spinlock_lock(&lock);      // 获取锁
 * spinlock_unlock(&lock);    // 释放锁
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SPINLOCK_T_H
#define _SPINLOCK_T_H

/**
 * @brief 自旋锁结构
 * 
 * 用于多核环境下保护短临界区
 * lock=0 表示未锁定，lock=1 表示已锁定
 */
typedef struct {
    volatile u32_t lock;    /**< 锁状态: 0=未锁定, 1=已锁定 */
} spinlock_t;

#endif
