/*
 * @Author: Jee Hsu
 * @Description: 信号量和等待队列类型定义
 *
 * 提供进程同步的数据结构：
 * - waitlist_t: 等待队列(阻塞进程链表)
 * - sem_t: 信号量(计数/互斥)
 * - wait_l_head_t: 等待链表头
 *
 * 信号量类型：
 * - SEM_FLG_MUTEX: 互斥信号量(二值)
 * - SEM_FLG_MULTI: 计数信号量
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SEM_T_H
#define _SEM_T_H

/** 信号量类型标志 */
#define SEM_FLG_MUTEX 0         /**< 互斥信号量(二值) */
#define SEM_FLG_MULTI 1         /**< 计数信号量 */
#define SEM_MUTEX_ONE_LOCK 1    /**< 互斥锁已锁定 */
#define SEM_MULTI_LOCK 0        /**< 计数信号量锁定值 */

/**
 * @brief 等待队列结构
 * 
 * 管理因资源不可用而阻塞的进程列表
 */
typedef struct s_KWLST {   
    spinlock_t wl_lock;     /**< 保护等待队列的自旋锁 */
    uint_t wl_tdnr;         /**< 等待中的进程数量 */
    list_h_t wl_list;       /**< 等待进程链表头 */
} waitlist_t;

/**
 * @brief 信号量结构
 * 
 * 用于进程间同步，支持互斥和计数两种模式
 */
typedef struct s_SEM {
    spinlock_t sem_lock;    /**< 保护信号量的自旋锁 */
    uint_t sem_flags;       /**< 信号量类型: SEM_FLG_MUTEX/MULTI */
    sint_t sem_count;       /**< 信号量计数值 */
    waitlist_t sem_waitlst; /**< 等待该信号量的进程队列 */
} sem_t;

/**
 * @brief 通用等待链表头结构
 * 
 * 用于实现各种等待机制的通用链表头
 */
typedef struct s_WAIT_L_HEAD {
    list_h_t wlh_llist;     /**< 链表节点 */
    spinlock_t wlh_lock;    /**< 保护链表的自旋锁 */
    atomic_t wlh_count;     /**< 等待者计数 */
    void* wlh_privte;       /**< 私有数据指针 */
    /** 唤醒回调函数 */
    bool_t (*wlh_upfun)(u32_t func, struct s_WAIT_L_HEAD* wlhp);
} wait_l_head_t;

/**
 * @brief 等待链表节点结构
 * 
 * 代表一个等待中的进程
 */
typedef struct s_WAIT_L {
    list_h_t wl_hlist;      /**< 链表节点 */
    u32_t wl_flags;         /**< 等待标志 */
    void* wl_thead;         /**< 等待的线程指针 */
} wait_l_t;

#endif
