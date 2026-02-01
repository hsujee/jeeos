/*
 * @Author: Jee Hsu
 * @Description: 物理内存区域管理类型定义
 *
 * 定义物理内存分区管理的核心数据结构：
 * - mmarea_t: 内存区域(硬件区/内核区/用户区)
 * - mmdivide_t: 伙伴系统分配器
 * - bafhlst_t: 伙伴系统页面链表
 *
 * 内存分区策略：
 * - 硬件区(0-32MB): DMA等硬件访问
 * - 内核区(32MB-16GB): 内核使用
 * - 用户区(16GB以上): 用户进程
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_MEM_ZONE_T_H
#define ARCH_X86_MEM_ZONE_T_H

#define MMSTUS_ERR (0)      /**< 操作失败 */
#define MMSTUS_OK (1)       /**< 操作成功 */

/**
 * @brief ARC页面缓存链表结构
 * 
 * 实现ARC(Adaptive Replacement Cache)算法
 */
typedef struct s_ARCLST {
    list_h_t al_lru1;       /**< LRU列表1(最近使用) */
    list_h_t al_lru2;       /**< LRU列表2(频繁使用) */
    uint_t al_lru1nr;       /**< LRU1中的页面数 */
    uint_t al_lru2nr;       /**< LRU2中的页面数 */
} arclst_t;

/**
 * @brief 内存分配返回结构
 */
typedef struct s_MMAFRETS {
    msadsc_t* mat_fist;     /**< 首个物理页描述符 */
    uint_t mat_sz;          /**< 分配大小 */
    uint_t mat_phyadr;      /**< 物理地址 */
    u16_t mat_runmode;      /**< 运行模式 */
    u16_t mat_gen;          /**< 代数 */
    u32_t mat_mask;         /**< 掩码 */
} __attribute__((packed)) mmafrets_t;

struct s_MMAREA;
/**
 * @brief 内存区域操作函数表
 * 
 * 允许每个内存区域使用不同的分配策略
 */
typedef struct s_MAFUNCOBJS {
    mmstus_t (*mafo_init)(struct s_MMAREA* mmarea, void* valp, uint_t val);
    mmstus_t (*mafo_exit)(struct s_MMAREA* mmarea);
    mmstus_t (*mafo_aloc)(struct s_MMAREA* mmarea, mmafrets_t* mafrspack, void* valp, uint_t val);
    mmstus_t (*mafo_free)(struct s_MMAREA* mmarea, mmafrets_t* mafrspack, void* valp, uint_t val);
    mmstus_t (*mafo_recy)(struct s_MMAREA* mmarea, mmafrets_t* mafrspack, void* valp, uint_t val);
} mafuncobjs_t;

/** @name 伙伴链表状态 */
/** @{ */
#define BAFH_STUS_INIT 0    /**< 初始状态 */
#define BAFH_STUS_ONEM 1    /**< 单页模式 */
#define BAFH_STUS_DIVP 2    /**< 分割模式 */
#define BAFH_STUS_DIVM 3    /**< 合并模式 */
/** @} */

/**
 * @brief 伙伴系统页面链表结构
 * 
 * 管理特定order大小的物理页块
 * order=n 表示 2^n 个连续页面
 */
typedef struct s_BAFHLST {
    spinlock_t af_lock;     /**< 保护链表的自旋锁 */
    u32_t af_status;          /**< 状态 */
    uint_t af_oder;         /**< 页面阶数(2^order个页面) */
    uint_t af_oderpnr;      /**< 该阶的页面数(1<<order) */
    uint_t af_fobjnr;       /**< 空闲页块数量 */
    uint_t af_mobjnr;       /**< 总页块数量 */
    uint_t af_alcindx;      /**< 分配计数 */
    uint_t af_freindx;      /**< 释放计数 */
    list_h_t af_frelst;     /**< 空闲页块链表 */
    list_h_t af_alclst;     /**< 已分配页块链表 */
    list_h_t af_ovelst;     /**< 溢出链表 */
} bafhlst_t;

/** @name 伙伴系统配置 */
/** @{ */
#define MDIVMER_ARR_LMAX 52     /**< 最大阶数数组大小 */
#define MDIVMER_ARR_BMAX 11     /**< 块数组大小 */
#define MDIVMER_ARR_OMAX 9      /**< 单页数组大小 */
/** @} */

/**
 * @brief 伙伴系统分配器结构
 * 
 * 管理内存的分割和合并操作
 */
typedef struct s_MMDIVIDE {
    spinlock_t dm_lock;     /**< 保护分配器的自旋锁 */
    u32_t dm_status;          /**< 状态 */
    uint_t dm_dmmaxindx;    /**< 最大索引 */
    uint_t dm_phydmindx;    /**< 物理索引 */
    uint_t dm_predmindx;    /**< 预测索引 */
    uint_t dm_divnr;        /**< 分割次数统计 */
    uint_t dm_mernr;        /**< 合并次数统计 */
    bafhlst_t dm_mdmlielst[MDIVMER_ARR_LMAX]; /**< 各阶伙伴链表数组 */
    bafhlst_t dm_onemsalst; /**< 单页链表 */
} mmdivide_t;

/** @name 内存区域类型 */
/** @{ */
#define MA_TYPE_INIT 0      /**< 未初始化 */
#define MA_TYPE_HWAD 1      /**< 硬件区(DMA) */
#define MA_TYPE_KRNL 2      /**< 内核区 */
#define MA_TYPE_PROC 3      /**< 用户区 */
#define MA_TYPE_SHAR 4      /**< 共享区 */
#define MMAREA_MAX 4        /**< 最大区域数 */
/** @} */

/** @name 硬件区地址范围(0-32MB) */
/** @{ */
#define MA_HWAD_LSTART 0
#define MA_HWAD_LSZ 0x2000000
#define MA_HWAD_LEND (MA_HWAD_LSTART + MA_HWAD_LSZ - 1)
/** @} */

/** @name 内核区地址范围(32MB-16GB) */
/** @{ */
#define MA_KRNL_LSTART 0x2000000
#define MA_KRNL_LSZ (0x400000000 - 0x2000000)
#define MA_KRNL_LEND (MA_KRNL_LSTART + MA_KRNL_LSZ - 1)
/** @} */

/** @name 用户区地址范围(16GB以上) */
/** @{ */
#define MA_PROC_LSTART 0x400000000
#define MA_PROC_LSZ (0xffffffffffffffff - 0x400000000)
#define MA_PROC_LEND (MA_PROC_LSTART + MA_PROC_LSZ)
/** @} */

/**
 * @brief 物理内存区域结构
 * 
 * 代表一个物理内存分区，每个分区可有独立的分配策略
 * 系统通常有3个区域：硬件区、内核区、用户区
 */
typedef struct s_MMAREA {
    list_h_t ma_list;           /**< 区域链表节点 */
    spinlock_t ma_lock;         /**< 保护区域的自旋锁 */
    uint_t ma_status;             /**< 区域状态 */
    uint_t ma_flags;            /**< 区域标志 */
    uint_t ma_type;             /**< 区域类型: MA_TYPE_xxx */
    sem_t ma_sem;               /**< 区域信号量 */
    wait_l_head_t ma_waitlst;   /**< 内存不足时的等待队列 */
    uint_t ma_maxpages;         /**< 总页面数 */
    uint_t ma_allocpages;       /**< 已分配页面数 */
    uint_t ma_freepages;        /**< 空闲页面数 */
    uint_t ma_resvpages;        /**< 保留页面数 */
    uint_t ma_horizline;        /**< 分配水位线(低于此值触发回收) */
    adr_t ma_logicstart;        /**< 逻辑起始地址 */
    adr_t ma_logicend;          /**< 逻辑结束地址 */
    uint_t ma_logicsz;          /**< 逻辑大小 */
    adr_t ma_effectstart;       /**< 有效起始地址 */
    adr_t ma_effectend;         /**< 有效结束地址 */
    uint_t ma_effectsz;         /**< 有效大小 */
    list_h_t ma_allmsadsclst;   /**< 所有页描述符链表 */
    uint_t ma_allmsadscnr;      /**< 页描述符数量 */
    arclst_t ma_arcpglst;       /**< ARC页面缓存 */
    mafuncobjs_t ma_funcobj;    /**< 分配操作函数表 */
    mmdivide_t ma_mdmdata;      /**< 伙伴系统分配器 */
    void* ma_privp;             /**< 私有数据 */
} mmarea_t;

#endif // ARCH_X86_MEM_ZONE_T_H