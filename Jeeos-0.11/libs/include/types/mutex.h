/*
 * @Author: Jee Hsu
 * @Description: 互斥锁操作函数
 *
 * 提供用户态互斥锁操作：
 * - mutex_init: 初始化互斥锁
 * - mutex_lock: 获取锁（阻塞）
 * - mutex_trylock: 尝试获取锁（非阻塞）
 * - mutex_unlock: 释放锁
 *
 * 使用原子操作实现自旋锁语义
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _MUTEX_H
#define _MUTEX_H

#include "mutex_t.h"
#include "atomic.h"

/*===========================================================================
 * 互斥锁操作
 *===========================================================================*/

/**
 * @brief 初始化互斥锁
 * @param m 互斥锁指针
 */
KLINE void mutex_init(mutex_t *m) {
    atomic_set(&m->state, MUTEX_UNLOCKED);
    m->owner = -1;
    m->count = 0;
}

/**
 * @brief 尝试获取互斥锁（非阻塞）
 * @param m 互斥锁指针
 * @return 成功返回1，失败返回0
 */
KLINE int mutex_trylock(mutex_t *m) {
    /* 尝试将状态从 UNLOCKED 改为 LOCKED */
    if (atomic_cmpxchg(&m->state, MUTEX_UNLOCKED, MUTEX_LOCKED) == MUTEX_UNLOCKED) {
        barrier();
        return 1;  /* 成功获取锁 */
    }
    return 0;  /* 锁已被占用 */
}

/**
 * @brief 获取互斥锁（阻塞等待）
 * @param m 互斥锁指针
 * @note 使用自旋等待，适用于短临界区
 */
KLINE void mutex_lock(mutex_t *m) {
    while (!mutex_trylock(m)) {
        /* 自旋等待，使用pause指令减少CPU占用 */
        __asm__ __volatile__("pause" ::: "memory");
    }
}

/**
 * @brief 释放互斥锁
 * @param m 互斥锁指针
 */
KLINE void mutex_unlock(mutex_t *m) {
    barrier();
    atomic_set(&m->state, MUTEX_UNLOCKED);
}

/**
 * @brief 检查互斥锁是否被锁定
 * @param m 互斥锁指针
 * @return 锁定返回1，未锁定返回0
 */
KLINE int mutex_is_locked(mutex_t *m) {
    return atomic_read(&m->state) == MUTEX_LOCKED;
}

/*===========================================================================
 * 条件变量操作 (简化实现)
 *===========================================================================*/

/**
 * @brief 初始化条件变量
 * @param cv 条件变量指针
 */
KLINE void condvar_init(condvar_t *cv) {
    atomic_set(&cv->waiters, 0);
    atomic_set(&cv->signal, 0);
}

/**
 * @brief 等待条件变量
 * @param cv 条件变量指针
 * @param m 关联的互斥锁
 * @note 调用前必须持有互斥锁，等待期间会释放锁
 */
KLINE void condvar_wait(condvar_t *cv, mutex_t *m) {
    atomic_inc(&cv->waiters);
    mutex_unlock(m);
    
    /* 等待信号 */
    while (atomic_read(&cv->signal) == 0) {
        __asm__ __volatile__("pause" ::: "memory");
    }
    
    atomic_dec(&cv->waiters);
    atomic_dec(&cv->signal);
    mutex_lock(m);
}

/**
 * @brief 唤醒一个等待线程
 * @param cv 条件变量指针
 */
KLINE void condvar_signal(condvar_t *cv) {
    if (atomic_read(&cv->waiters) > 0) {
        atomic_inc(&cv->signal);
    }
}

/**
 * @brief 唤醒所有等待线程
 * @param cv 条件变量指针
 */
KLINE void condvar_broadcast(condvar_t *cv) {
    s32_t waiters = atomic_read(&cv->waiters);
    while (waiters-- > 0) {
        atomic_inc(&cv->signal);
    }
}

#endif /* _MUTEX_H */

