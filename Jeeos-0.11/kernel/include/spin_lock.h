/*
 * @Author: Jee Hsu
 * @Description: 自旋锁实现 - 底层同步原语
 *
 * 提供内核级自旋锁操作，用于短期临界区保护
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SPINLOCK_H
#define _SPINLOCK_H

/**
 * @brief 初始化自旋锁
 * @param lock 自旋锁指针
 */
void spinlock_init(spinlock_t* lock);

/**
 * @brief 获取自旋锁
 * @param lock 自旋锁指针
 */
void spinlock_lock(spinlock_t* lock);

/**
 * @brief 释放自旋锁
 * @param lock 自旋锁指针
 */
void spinlock_unlock(spinlock_t* lock);

/**
 * @brief 关中断并获取自旋锁
 * @param lock 自旋锁指针
 * @param cpuflg 保存CPU标志的指针
 */
void spinlock_cli(spinlock_t* lock, cpuflg_t* cpuflg);

/**
 * @brief 释放自旋锁并恢复中断
 * @param lock 自旋锁指针
 * @param cpuflg 保存的CPU标志指针
 */
void spinunlock_sti(spinlock_t* lock, cpuflg_t* cpuflg);

#endif // _SPINLOCK_H
