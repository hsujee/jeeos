/*
 * @Author: Jee Hsu
 * @Description: 物理页面描述符类型定义
 *
 * 定义物理内存页面管理的核心数据结构：
 * - msadsc_t: 物理页描述符(每个4KB页面一个)
 * - msadflgs_t: 页面管理标志
 * - phyadrflgs_t: 物理地址和页面状态标志
 *
 * 物理页描述符是内存管理的基础，记录每个物理页的状态
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_PAGE_DESC_T_H
#define ARCH_X86_PAGE_DESC_T_H

#define PAGPHYADR_SZLSHBIT (12) /**< 页地址左移位数(4KB=2^12) */
#define MSAD_PAGE_MAX (8)       /**< 最大连续页数 */
#define MSA_SIZE (1 << PAGPHYADR_SZLSHBIT) /**< 页大小: 4KB */

/** @name 页面链表类型 */
/** @{ */
#define MF_OLKTY_INIT (0)   /**< 初始状态 */
#define MF_OLKTY_ODER (1)   /**< 按order挂载 */
#define MF_OLKTY_BAFH (2)   /**< 在伙伴链表中 */
#define MF_OLKTY_TOBJ (3)   /**< 在对象链表中 */
/** @} */

/** @name 页面占用类型 */
/** @{ */
#define MF_LSTTY_LIST (0)   /**< 链表类型 */
#define MF_MOCTY_FREE (0)   /**< 空闲页 */
#define MF_MOCTY_KRNL (1)   /**< 内核占用 */
#define MF_MOCTY_USER (2)   /**< 用户占用 */
#define MF_MRV1_VAL (0)
#define MF_UINDX_INIT (0)   /**< 分配计数初始值 */
#define MF_UINDX_MAX (0xffffff) /**< 分配计数最大值 */
/** @} */

/** @name 页面所属区域类型 */
/** @{ */
#define MF_MARTY_INIT (0)   /**< 初始状态 */
#define MF_MARTY_HWD (1)    /**< 硬件区(DMA) */
#define MF_MARTY_KRL (2)    /**< 内核区 */
#define MF_MARTY_PRC (3)    /**< 用户区 */
#define MF_MARTY_SHD (4)    /**< 共享区 */
/** @} */

/**
 * @brief 页面管理标志位域结构
 */
typedef struct s_MSADFLGS {
    u32_t mf_olkty:2;       /**< 链表类型: MF_OLKTY_xxx */
    u32_t mf_lstty:1;       /**< 是否在链表中: 0=否, 1=是 */
    u32_t mf_mocty:2;       /**< 占用类型: MF_MOCTY_xxx */
    u32_t mf_marty:3;       /**< 所属区域: MF_MARTY_xxx */
    u32_t mf_uindx:24;      /**< 分配计数(引用计数) */
} __attribute__((packed)) msadflgs_t; 

/** @name 页面状态标志 */
/** @{ */
#define PAF_NO_ALLOC (0)    /**< 未分配 */
#define PAF_ALLOC (1)       /**< 已分配 */
#define PAF_NO_SHARED (0)   /**< 未共享 */
#define PAF_NO_SWAP (0)     /**< 不可换出 */
#define PAF_NO_CACHE (0)    /**< 无缓存 */
#define PAF_NO_KMAP (0)     /**< 未内核映射 */
#define PAF_NO_LOCK (0)     /**< 未锁定 */
#define PAF_NO_DIRTY (0)    /**< 非脏页 */
#define PAF_NO_BUSY (0)     /**< 非忙状态 */
#define PAF_RV2_VAL (0)
#define PAF_INIT_PADRS (0)  /**< 物理地址初始值 */
/** @} */

/**
 * @brief 物理地址和状态标志位域结构
 */
typedef struct s_PHYADRFLGS {
    u64_t paf_alloc:1;      /**< 分配标志: 0=空闲, 1=已分配 */
    u64_t paf_shared:1;     /**< 共享标志: 1=多个进程共享 */
    u64_t paf_swap:1;       /**< 可换出标志 */
    u64_t paf_cache:1;      /**< 在缓存中标志 */
    u64_t paf_kmap:1;       /**< 内核映射标志 */
    u64_t paf_lock:1;       /**< 锁定标志(禁止换出) */
    u64_t paf_dirty:1;      /**< 脏页标志(需写回) */
    u64_t paf_busy:1;       /**< 忙标志(正在操作) */
    u64_t paf_rv2:4;        /**< 保留 */
    u64_t paf_padrs:52;     /**< 物理页帧号(右移12位) */
} __attribute__((packed)) phyadrflgs_t;

/**
 * @brief 物理页描述符结构
 * 
 * 系统中每个4KB物理页面对应一个msadsc_t
 * 用于追踪物理页面的分配状态和属性
 */
typedef struct s_MSADSC {
    list_h_t md_list;       /**< 链表节点(16字节) */
    spinlock_t md_lock;     /**< 保护页描述符的自旋锁(4字节) */
    msadflgs_t md_indxflags; /**< 管理标志(4字节) */
    phyadrflgs_t md_phyadrs;/**< 物理地址和状态(8字节) */
    void* md_odlink;        /**< 伙伴页指针(相邻且相同order)(8字节) */
} __attribute__((packed)) msadsc_t;

#endif // ARCH_X86_PAGE_DESC_T_H