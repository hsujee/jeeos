/*
 * @Author: Jee Hsu
 * @Description: 虚拟地址空间管理类型定义
 *
 * 定义进程虚拟地址空间管理的核心数据结构：
 * - mmadrsdsc_t: 内存地址空间描述符(每进程一份)
 * - virmemadrs_t: 虚拟地址区间管理器
 * - kmvarsdsc_t: 虚拟地址区间描述符(VMA)
 * - kvmemcbox_t: 内核虚拟内存盒(管理物理页映射)
 *
 * 层次结构：
 *   mmadrsdsc_t -> virmemadrs_t -> kmvarsdsc_t[] -> kvmemcbox_t
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _VIRADDRESS_T_H
#define _VIRADDRESS_T_H

/** @name 页表层级返回值 */
/** @{ */
#define RET4PT_PML4EVDR (1)     /**< PML4层级 */
#define RET4PT_PDPTEVDR (2)     /**< PDPT层级 */
#define RET4PT_PDETEVDR (3)     /**< PD层级 */
#define RET4PT_PTETEVDR (4)     /**< PT层级 */
#define RET4PT_PFAMEPDR (5)     /**< 物理页帧 */
/** @} */

#define VMAP_MIN_SIZE (MSA_SIZE)    /**< 最小映射大小 */

/** @name 内存盒缓存配置 */
/** @{ */
#define KMBOX_CACHE_MAX (0x1000)    /**< 最大缓存数量 */
#define KMBOX_CACHE_MIN (0x40)      /**< 最小缓存数量 */
/** @} */

/**
 * @brief 内核虚拟内存对象管理器
 * 
 * 管理所有kvmemcobj_t实例
 */
typedef struct KVMCOBJMGR {
    spinlock_t kom_lock;        /**< 保护管理器的自旋锁 */
    u32_t kom_flags;            /**< 标志 */
    uint_t kom_kvmcobjnr;       /**< 对象数量 */
    list_h_t kom_kvmcohead;     /**< 对象链表头 */
    uint_t kom_kvmcocahenr;     /**< 缓存对象数量 */
    list_h_t kom_kvmcocahe;     /**< 缓存链表 */
    uint_t kom_kvmcodelnr;      /**< 待删除对象数量 */
    list_h_t kom_kvmcodelhead;  /**< 待删除链表 */
} kvmcobjmgr_t;

/**
 * @brief 内核虚拟内存对象结构
 * 
 * 代表一个可共享的内存对象(如文件映射)
 */
typedef struct KVMEMCOBJ {
    list_h_t kco_list;          /**< 链表节点 */
    spinlock_t kco_lock;        /**< 保护对象的自旋锁 */
    u32_t kco_cont;             /**< 引用计数 */
    u32_t kco_flags;            /**< 标志 */
    u32_t kco_type;             /**< 对象类型 */
    uint_t kco_msadnr;          /**< 物理页描述符数量 */
    list_h_t kco_msadlst;       /**< 物理页描述符链表 */
    void* kco_filenode;         /**< 关联的文件节点 */
    void* kco_pager;            /**< 分页器(用于换页) */
    void* kco_extp;             /**< 扩展数据 */
} kvmemcobj_t;

/**
 * @brief 内核虚拟内存盒管理器
 * 
 * 管理所有kvmemcbox_t实例及其缓存
 */
typedef struct KVMEMCBOXMGR {
    list_h_t kbm_list;          /**< 链表节点 */
    spinlock_t kbm_lock;        /**< 保护管理器的自旋锁 */
    u64_t kbm_flags;            /**< 标志 */
    u64_t kbm_status;             /**< 状态 */
    uint_t kbm_kmbnr;           /**< 内存盒数量 */
    list_h_t kbm_kmbhead;       /**< 内存盒链表头 */
    uint_t kbm_cachenr;         /**< 缓存的空闲内存盒数量 */
    uint_t kbm_cachemax;        /**< 缓存上限 */
    uint_t kbm_cachemin;        /**< 缓存下限 */
    list_h_t kbm_cachehead;     /**< 缓存链表头 */
    void* kbm_ext;              /**< 扩展数据 */
} kvmemcboxmgr_t;

/**
 * @brief 内核虚拟内存盒结构
 * 
 * 管理虚拟地址区间到物理页面的映射
 * 可被多个VMA共享(如fork后的COW页面)
 */
typedef struct KVMEMCBOX {
    list_h_t kmb_list;          /**< 链表节点 */
    spinlock_t kmb_lock;        /**< 保护内存盒的自旋锁 */
    refcount_t kmb_cont;        /**< 引用计数(支持共享) */
    u64_t kmb_flags;            /**< 标志 */
    u64_t kmb_status;             /**< 状态 */
    u64_t kmb_type;             /**< 类型 */
    uint_t kmb_msanr;           /**< 物理页描述符数量 */
    list_h_t kmb_msalist;       /**< 物理页描述符链表 */
    kvmemcboxmgr_t* kmb_mgr;    /**< 所属管理器 */
    void* kmb_filenode;         /**< 关联的文件节点 */
    void* kmb_pager;            /**< 分页器(预留) */
    void* kmb_ext;              /**< 扩展数据 */
} kvmemcbox_t;

/**
 * @brief 虚拟地址区间红黑树节点
 */
typedef struct VASLKNODE {
    u32_t vln_color;            /**< 节点颜色(红/黑) */
    u32_t vln_flags;            /**< 标志 */
    void* vln_left;             /**< 左子节点 */
    void* vln_right;            /**< 右子节点 */
    void* vln_prev;             /**< 前驱节点 */
    void* vln_next;             /**< 后继节点 */
} vaslknode_t;

/**
 * @brief 测试用结构(调试用)
 */
typedef struct TESTSTC {
    list_h_t tst_list;          /**< 链表节点 */
    adr_t tst_vadr;             /**< 虚拟地址 */
    size_t tst_vsiz;            /**< 大小 */
    uint_t tst_type;            /**< 类型 */
    uint_t tst_lime;            /**< 限制 */
} teststc_t;

/**
 * @brief 页表页面管理结构
 */
typedef struct PGTABPAGE {
    spinlock_t ptp_lock;        /**< 保护页表页的自旋锁 */
    list_h_t ptp_msalist;       /**< 页表页的物理页链表 */
    uint_t ptp_msanr;           /**< 页表页数量 */
} pgtabpage_t;

/**
 * @brief 虚拟地址区间描述符(VMA)
 * 
 * 描述进程虚拟地址空间中的一段连续区域
 * 类似Linux的vm_area_struct
 */
typedef struct KMVARSDSC {
    spinlock_t kva_lock;        /**< 保护VMA的自旋锁 */
    u32_t kva_maptype;          /**< 映射类型(私有/共享) */
    list_h_t kva_list;          /**< VMA链表节点 */
    u64_t kva_flags;            /**< 访问权限标志 */
    u64_t kva_limits;           /**< 限制 */
    vaslknode_t kva_lknode;     /**< 红黑树节点(快速查找) */
    void* kva_mcstruct;         /**< 所属virmemadrs_t */
    adr_t kva_start;            /**< 区间起始虚拟地址 */
    adr_t kva_end;              /**< 区间结束虚拟地址 */
    kvmemcbox_t* kva_kvmbox;    /**< 物理页映射管理 */
    void* kva_kvmcobj;          /**< 关联的内存对象 */
} kmvarsdsc_t;

/**
 * @brief 内核虚拟地址空间管理结构
 * 
 * 管理内核地址空间的VMA
 */
typedef struct KVIRMEMADRS {
    spinlock_t kvs_lock;        /**< 保护的自旋锁 */
    u64_t kvs_flags;            /**< 标志 */
    uint_t kvs_kmvdscnr;        /**< VMA数量 */
    kmvarsdsc_t* kvs_startkmvdsc;   /**< 起始VMA */
    kmvarsdsc_t* kvs_endkmvdsc;     /**< 结束VMA */
    kmvarsdsc_t* kvs_krlmapdsc;     /**< 内核映射VMA */
    kmvarsdsc_t* kvs_krlhwmdsc;     /**< 硬件映射VMA */
    kmvarsdsc_t* kvs_krlolddsc;     /**< 旧映射VMA */
    adr_t kvs_isalcstart;       /**< 可分配起始地址 */
    adr_t kvs_isalcend;         /**< 可分配结束地址 */
    void* kvs_privte;           /**< 私有数据 */
    void* kvs_ext;              /**< 扩展数据 */
    list_h_t kvs_testhead;      /**< 测试链表 */
    uint_t kvs_tstcnr;          /**< 测试计数 */
    uint_t kvs_randnext;        /**< 随机数 */
    pgtabpage_t kvs_ptabpgcs;   /**< 页表页管理 */
    kvmcobjmgr_t kvs_kvmcomgr;  /**< 内存对象管理器 */
    kvmemcboxmgr_t kvs_kvmemcboxmgr; /**< 内存盒管理器 */
} kvirmemadrs_t;

typedef struct s_MMADRSDSC mmadrsdsc_t;

/**
 * @brief 用户进程虚拟地址空间管理结构
 * 
 * 管理用户进程的所有VMA
 */
typedef struct s_VIRMEMADRS {
    spinlock_t vs_lock;         /**< 保护的自旋锁 */
    u32_t vs_resalin;           /**< 对齐保留 */
    list_h_t vs_list;           /**< VMA链表头 */
    uint_t vs_flags;            /**< 标志 */
    uint_t vs_kmvdscnr;         /**< VMA数量 */
    mmadrsdsc_t* vs_mm;         /**< 所属地址空间描述符 */
    kmvarsdsc_t* vs_startkmvdsc;    /**< 起始VMA */
    kmvarsdsc_t* vs_endkmvdsc;      /**< 结束VMA */
    kmvarsdsc_t* vs_currkmvdsc;     /**< 当前VMA(用于分配) */
    kmvarsdsc_t* vs_krlmapdsc;      /**< 内核映射区VMA */
    kmvarsdsc_t* vs_krlhwmdsc;      /**< 硬件映射区VMA */
    kmvarsdsc_t* vs_krlolddsc;      /**< 旧映射区VMA */
    adr_t vs_isalcstart;        /**< 可分配起始地址 */
    adr_t vs_isalcend;          /**< 可分配结束地址 */
    void* vs_privte;            /**< 私有数据 */
    void* vs_ext;               /**< 扩展数据 */
} virmemadrs_t;

/**
 * @brief 内存地址空间描述符(每进程唯一)
 * 
 * 进程级别的内存管理核心结构
 * 包含MMU配置和虚拟地址空间布局
 */
typedef struct s_MMADRSDSC {
    spinlock_t msd_lock;        /**< 保护的自旋锁 */
    list_h_t msd_list;          /**< 链表节点 */
    uint_t msd_flag;            /**< 标志 */
    uint_t msd_status;            /**< 状态 */
    uint_t msd_scount;          /**< 共享计数 */
    sem_t msd_sem;              /**< 信号量 */
    mmudsc_t msd_mmu;           /**< MMU描述符(页表) */
    virmemadrs_t msd_virmemadrs;/**< 虚拟地址空间管理器 */
    adr_t msd_stext;            /**< 代码段起始地址 */
    adr_t msd_etext;            /**< 代码段结束地址 */
    adr_t msd_sdata;            /**< 数据段起始地址 */
    adr_t msd_edata;            /**< 数据段结束地址 */
    adr_t msd_sbss;             /**< BSS段起始地址 */
    adr_t msd_ebss;             /**< BSS段结束地址 */
    adr_t msd_sbrk;             /**< 堆区起始地址 */
    adr_t msd_ebrk;             /**< 堆区结束地址(brk) */
} mmadrsdsc_t;

#define VADSZ_ALIGN(x) ALIGN(x, 0x1000) /**< 虚拟地址页对齐 */
#define KVMCOBJ_FLG_DELLPAGE (1)        /**< 删除页面标志 */
#define KVMCOBJ_FLG_UDELPAGE (2)        /**< 用户删除页面标志 */

#endif