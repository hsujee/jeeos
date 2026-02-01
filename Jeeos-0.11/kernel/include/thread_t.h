/*
 * @Author: Jee Hsu
 * @Description: 线程/进程管理类型定义
 *
 * 定义内核线程管理的核心数据结构：
 * - thread_t: 线程控制块(TCB)
 * - context_t: CPU上下文
 * - micrstk_t: 微内核栈
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _THREAD_T_H
#define _THREAD_T_H

/** @name 线程状态定义 */
/** @{ */
#define TDSTUS_RUN 0        /**< 运行态: 正在CPU上执行 */
#define TDSTUS_SLEEP 3      /**< 睡眠态: 主动休眠 */
#define TDSTUS_WAIT 4       /**< 等待态: 等待资源/事件 */
#define TDSTUS_NEW 5        /**< 新建态: 刚创建未运行 */
#define TDSTUS_ZOMB 6       /**< 僵尸态: 已结束待回收 */
/** @} */

/** @name 线程标志定义 */
/** @{ */
#define TDFLAG_FREE (1)     /**< 线程空闲 */
#define TDFLAG_BUSY (2)     /**< 线程忙碌 */
/** @} */

#define TDRUN_TICK 20       /**< 默认时间片(tick数) */

/** @name 优先级定义 */
/** @{ */
#define PRITY_MAX 64        /**< 最大优先级 */
#define PRITY_MIN 0         /**< 最小优先级 */
#define PRILG_SYS 0         /**< 系统级特权 */
#define PRILG_USR 5         /**< 用户级特权 */
/** @} */

#define MICRSTK_MAX 4       /**< 微栈最大深度 */
#define THREAD_MAX (4)      /**< 最大线程数 */

#define KERNTHREAD_FLG 0    /**< 内核线程标志 */
#define USERTHREAD_FLG 3    /**< 用户线程标志 */

#if((defined CFG_X86_PLATFORM)) 
#define DAFT_TDUSRSTKSZ 0x8000  /**< 默认用户栈大小: 32KB */
#define DAFT_TDKRLSTKSZ 0x8000  /**< 默认内核栈大小: 32KB */
#endif

#if((defined CFG_X86_PLATFORM)) 
#define TD_HAND_MAX 8       /**< 线程最大打开句柄数 */
#define DAFT_SPSR 0x10
#define DAFT_CPSR 0xd3
#define DAFT_CIDLESPSR 0x13   
#endif

/** @name 段选择子定义 */
/** @{ */
#define K_CS_IDX    0x08    /**< 内核代码段 */
#define K_DS_IDX    0x10    /**< 内核数据段 */
#define U_CS_IDX    0x1b    /**< 用户代码段 */
#define U_DS_IDX    0x23    /**< 用户数据段 */
#define K_TAR_IDX   0x28    /**< TSS段 */
#define UMOD_EFLAGS 0x1202  /**< 用户态EFLAGS */
#define KMOD_EFLAGS 0x202   /**< 内核态EFLAGS */
/** @} */

/**
 * @brief 微内核栈结构
 */
typedef struct s_MICRSTK {
    uint_t msk_val[MICRSTK_MAX];    /**< 栈数据 */
} micrstk_t;

/**
 * @brief CPU上下文结构
 * 
 * 保存线程切换时的CPU状态
 */
typedef struct s_CONTEXT {  
    uint_t ctx_nextrip;     /**< 下次执行的指令地址(RIP) */
    uint_t ctx_nextrsp;     /**< 下次执行的内核栈指针(RSP) */
    x64tss_t* ctx_nexttss;  /**< TSS结构指针 */
} context_t;

/**
 * @brief 线程控制块(TCB)
 * 
 * 内核中代表一个线程/进程的核心数据结构
 * 包含线程状态、调度信息、栈空间、地址空间等
 */
typedef struct s_THREAD {
    spinlock_t td_lock;         /**< 线程自旋锁 */
    list_h_t td_list;           /**< 链表节点(用于调度队列) */
    uint_t td_flags;            /**< 线程标志 */
    uint_t td_status;             /**< 线程状态: TDSTUS_xxx */
    uint_t td_cpuid;            /**< 运行的CPU ID */
    uint_t td_id;               /**< 线程ID(全局唯一) */
    uint_t td_tick;             /**< 已运行的tick数 */
    uint_t td_privilege;        /**< 特权级: PRILG_SYS/USR */
    uint_t td_priority;         /**< 优先级: 0-63 */
    uint_t td_runmode;          /**< 运行模式: 内核/用户 */
    adr_t td_krlstktop;         /**< 内核栈顶地址 */
    adr_t td_krlstkstart;       /**< 内核栈起始地址 */
    adr_t td_usrstktop;         /**< 用户栈顶地址 */
    adr_t td_usrstkstart;       /**< 用户栈起始地址 */
    mmadrsdsc_t* td_mmdsc;      /**< 地址空间描述符 */
    void* td_resdsc;            /**< 资源描述符 */
    void* td_privtep;           /**< 私有数据指针 */
    void* td_extdatap;          /**< 扩展数据指针 */
    context_t td_context;       /**< CPU上下文 */
    objnode_t* td_handtbl[TD_HAND_MAX]; /**< 打开的对象句柄表 */
} thread_t;

#endif // THREAD_T_H
