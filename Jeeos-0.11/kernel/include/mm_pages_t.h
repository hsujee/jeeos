/*
 * @Author: Jee Hsu
 * @Description: 内核内存页面池类型定义
 *
 * 定义内核内存分配器的核心数据结构：
 * - pglmap_t: 页面映射节点
 * - mplhead_t: 内存池头部
 * - kmempool_t: 内核内存池(全局唯一)
 *
 * 内存分配策略：
 * - 页面分配: 以4KB为单位
 * - 对象分配: 小对象使用对象池
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */

#ifndef _MMPAGES_T_H
#define _MMPAGES_T_H

/** @name 内存池配置 */
/** @{ */
#define MPALCLST_MAX 5          /**< 分配列表最大数量 */
#define PMPLMAP_MAX 32          /**< 页面映射最大数量 */
#define KMEMPALCSZ_MIN 1        /**< 最小分配大小 */
#define KMEMPALCSZ_MAX 0x400000 /**< 最大分配大小: 4MB */
#define OBJSORPAGE 2048         /**< 每页对象数阈值 */
#define KPMPORHALM (PAGE_SIZE*31) /**< 半满阈值 */
/** @} */

/** @name 内存池类型 */
/** @{ */
#define MPLHTY_PAGE 1           /**< 页面类型内存池 */
#define MPLHTY_OBJS 2           /**< 对象类型内存池 */
/** @} */

#define OBJS_ALIGN(x) ALIGN(x,4)    /**< 对象4字节对齐 */
#define PHYMSA_MAX 512              /**< 物理内存区域最大数量 */
#define PAGE_SIZE 0x1000            /**< 页面大小: 4KB */
#define PAGE_SZRBIT (12)            /**< 页面大小位移量 */

/**
 * @brief 页面映射节点结构
 * 
 * 用于追踪内存池中的空闲页面
 */
typedef struct s_PGLMAP {
    adr_t pgl_start;            /**< 页面起始地址 */
    struct s_PGLMAP* pgl_next;  /**< 下一个映射节点 */
} pglmap_t;

/**
 * @brief 内存池头部结构
 * 
 * 管理一块连续的内存区域，用于页面或对象分配
 */
typedef struct s_MPLHEAD {
    spinlock_t mh_lock;         /**< 保护内存池的自旋锁 */
    list_h_t mh_list;           /**< 链表节点 */
    uint_t mh_hedty;            /**< 类型: MPLHTY_PAGE/OBJS */
    adr_t mh_start;             /**< 内存池起始地址 */
    adr_t mh_end;               /**< 内存池结束地址 */
    adr_t mh_firtfreadr;        /**< 第一个空闲地址 */
    pglmap_t* mh_firtpmap;      /**< 第一个页面映射 */
    uint_t mh_objnr;            /**< 对象数量 */
    uint_t mh_aliobsz;          /**< 对齐后的对象大小 */
    uint_t mh_objsz;            /**< 原始对象大小 */
    uint_t mh_nxtpsz;           /**< 下一块大小 */
    uint_t mh_afindx;           /**< 分配索引 */
    uint_t mh_pmnr;             /**< 页面映射数量 */
    pglmap_t* mh_pmap;          /**< 页面映射数组 */
} mplhead_t;

/**
 * @brief 内存区域头部结构
 * 
 * 管理物理内存区域的分配状态
 */
typedef struct s_MSAHEAD {
    uint_t mlh_nr;              /**< 区域内页面数量 */
    list_h_t mlh_msalst;        /**< 页面描述符链表 */
} msahead_t;

/**
 * @brief 内核内存池结构(全局唯一)
 * 
 * 系统级内存分配器，管理所有可用内存
 */
typedef struct s_KMEMPOOL {
    spinlock_t mp_lock;         /**< 全局锁 */
    list_h_t mp_list;           /**< 链表节点 */
    uint_t mp_status;             /**< 状态 */
    uint_t mp_flags;            /**< 标志 */
    spinlock_t mp_pglock;       /**< 页面池锁 */
    spinlock_t mp_oblock;       /**< 对象池锁 */
    uint_t mp_pgmplnr;          /**< 页面池数量 */
    uint_t mp_obmplnr;          /**< 对象池数量 */
    list_h_t mp_pgmplmheadl;    /**< 页面池链表 */
    list_h_t mp_obmplmheadl;    /**< 对象池链表 */
    mplhead_t* mp_pgmplmhcach;  /**< 页面池缓存 */
    mplhead_t* mp_obmplmhcach;  /**< 对象池缓存 */
#ifdef CFG_X86_PLATFORM
    msahead_t mp_msalsthead[PHYMSA_MAX]; /**< 物理区域头部数组 */
#endif
} kmempool_t;

#endif // MMPAGES_T_H
