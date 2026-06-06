/*
 * @Author: Jee Hsu
 * @Description: 进程调度器类型定义
 *
 * 定义多级反馈队列调度器的核心数据结构：
 * - thrdlst_t: 优先级线程链表
 * - schdata_t: 单CPU调度数据
 * - schedclass_t: 全局调度器类
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SCHED_T_H
#define _SCHED_T_H

/** @name 调度标志定义 */
/** @{ */
#define NOTS_SCHED_FLGS (0)             /**< 不需要调度 */
#define NEED_SCHED_FLGS (1)             /**< 需要调度 */
#define NEED_START_CPUILDE_SCHED_FLGS (2) /**< 需要启动CPU空闲进程 */
#define PMPT_FLGS 0                     /**< 抢占标志 */
/** @} */

#ifdef CFG_X86_PLATFORM
#define TNCCALL __attribute__((regparm(2)))         
#endif
#if((defined CFG_S3C2440A_PLATFORM) || (defined CFG_STM32F0XX_PLATFORM))
#define TNCCALL 
#endif

/**
 * @brief 线程链表结构(按优先级组织)
 * 
 * 每个优先级对应一个thrdlst_t，保存该优先级的所有就绪线程
 */
typedef struct s_THRDLST {
    list_h_t tdl_lsth;          /**< 线程链表头 */
    thread_t* tdl_curruntd;     /**< 该优先级当前运行的线程 */
    uint_t tdl_nr;              /**< 该优先级的线程数量 */
} thrdlst_t;

/**
 * @brief 单CPU调度数据结构
 * 
 * 每个CPU维护独立的调度数据，减少多核竞争
 */
typedef struct s_SCHDATA {
    spinlock_t sda_lock;        /**< 调度数据自旋锁 */
    uint_t sda_cpuid;           /**< CPU ID */
    uint_t sda_schedflags;        /**< 调度标志: NOTS/NEED_SCHED_FLGS */
    uint_t sda_premptidx;       /**< 抢占计数(>0时禁止抢占) */
    uint_t sda_threadnr;        /**< 该CPU上的线程总数 */
    uint_t sda_prityidx;        /**< 当前调度的优先级索引 */
    thread_t* sda_cpuidle;      /**< CPU空闲线程(idle) */
    thread_t* sda_currtd;       /**< 当前正在运行的线程 */
    thrdlst_t sda_thdlst[PRITY_MAX]; /**< 64级优先级队列 */
} schdata_t;

/**
 * @brief 全局调度器类
 * 
 * 系统唯一的调度器实例，管理所有CPU的调度数据
 */
typedef struct s_SCHEDCALSS {
    spinlock_t scls_lock;       /**< 全局调度器锁 */
    uint_t scls_cpunr;          /**< 系统CPU数量 */
    uint_t scls_threadnr;       /**< 系统线程总数 */
    uint_t scls_threadid_inc;   /**< 线程ID分配计数器 */
    schdata_t scls_schda[CPUCORE_MAX]; /**< 每CPU调度数据数组 */
} schedclass_t;

#endif // SCHED_T_H
