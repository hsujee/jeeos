/*
 * @Author: Jee Hsu
 * @Description: 中断/异常处理类型定义
 *
 * 定义中断管理框架的核心数据结构：
 * - int_desc_t: 中断描述符(每个中断号一个)
 * - int_server_t: 中断服务例程
 * - kithread_t: 内核中断线程
 *
 * 中断处理支持两种模式：
 * - 回调函数模式: 直接在中断上下文执行
 * - 中断线程模式: 在线程上下文执行(更灵活)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */

#ifndef ARCH_X86_ARCH_TRAP_T_H
#define ARCH_X86_ARCH_TRAP_T_H

#ifdef CFG_X86_PLATFORM

/**
 * @brief 中断线描述符
 */
typedef struct s_ILNEDSC {
    u32_t ild_modflag;       /**< 模式标志 */
    u32_t ild_devid;        /**< 设备ID */
    u32_t ild_physid;       /**< 物理ID */
    u32_t ild_clxsubinr;    /**< 子中断号 */
} ilnedsc_t;

/**
 * @brief 中断描述符结构
 * 
 * 每个中断号对应一个中断描述符
 * 管理该中断的所有处理程序
 */
typedef struct s_INTFLTDSC {
    spinlock_t i_lock;      /**< 保护描述符的自旋锁 */
    u32_t i_flags;          /**< 中断标志 */
    u32_t i_status;           /**< 中断状态 */
    uint_t i_prity;         /**< 中断优先级 */
    uint_t i_irqnr;         /**< 中断号(IRQ) */
    uint_t i_deep;          /**< 中断嵌套深度 */
    u64_t i_indx;           /**< 中断触发计数 */
    list_h_t i_serlist;     /**< 中断服务例程链表 */
    uint_t i_sernr;         /**< 服务例程数量 */
    list_h_t i_serthrdlst;  /**< 中断线程链表 */
    uint_t i_serthrdnr;     /**< 中断线程数量 */
    void* i_onethread;      /**< 单个中断线程指针(优化) */
    void* i_rbtreeroot;     /**< 中断线程红黑树(多线程时) */
    list_h_t i_serfisrlst;  /**< 快速中断服务链表(回调模式) */
    uint_t i_serfisrnr;     /**< 快速中断服务数量 */
    void* i_msgmpool;       /**< 中断消息池 */
    void* i_privp;          /**< 私有数据 */
    void* i_extp;           /**< 扩展数据 */
} int_desc_t;

/**
 * @brief 中断服务例程结构
 * 
 * 代表一个注册到中断描述符的处理程序
 */
typedef struct s_INTSERDSC {
    list_h_t s_list;        /**< 在中断描述符中的链表节点 */
    list_h_t s_indevlst;    /**< 在设备描述符中的链表节点 */
    u32_t s_flags;          /**< 服务标志 */
    int_desc_t* s_intfltp;  /**< 所属中断描述符 */
    void* s_device;         /**< 关联的设备 */
    uint_t s_indx;          /**< 执行计数 */
    int_handler_t s_handle; /**< 中断处理回调函数 */
} int_server_t;

/**
 * @brief 内核中断线程结构
 * 
 * 用于在线程上下文中处理中断
 * 支持更复杂的中断处理逻辑
 */
typedef struct s_KITHREAD {
    spinlock_t kit_lock;    /**< 保护线程的自旋锁 */
    list_h_t kit_list;      /**< 线程链表节点 */
    u32_t kit_flags;        /**< 线程标志 */
    u32_t kit_status;         /**< 线程状态 */
    uint_t kit_prity;       /**< 线程优先级 */
    u64_t kit_scdidx;       /**< 调度索引 */
    uint_t kit_runmode;     /**< 运行模式 */
    uint_t kit_cpuid;       /**< 运行的CPU ID */
    u16_t kit_cs;           /**< 代码段选择子 */
    u16_t kit_ss;           /**< 栈段选择子 */
    uint_t kit_nxteip;      /**< 下一条指令地址 */
    uint_t kit_nxtesp;      /**< 下一栈指针 */
    void* kit_stk;          /**< 线程栈 */
    size_t kit_stksz;       /**< 栈大小 */
    void* kit_runadr;       /**< 入口地址 */
    void* kit_binmodadr;    /**< 二进制模块地址 */
    void* kit_mmdsc;        /**< 内存描述符 */
    void* kit_privp;        /**< 私有数据 */
    void* kit_extp;         /**< 扩展数据 */
} kithread_t;

#endif

#endif // ARCH_X86_ARCH_TRAP_T_H
