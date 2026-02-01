/*
 * @Author: Jee Hsu
 * @Description: 架构层内存管理类型定义
 *
 * 定义x86架构的物理内存管理数据结构：
 * - phymmarge_t: 物理内存区域描述符
 * - phyadrspce_t: 物理地址空间描述符
 * - mmapdsc_t: 内存映射描述符
 * - alcfrelst_t: 分配释放链表
 * - phymem_t: 物理内存管理器
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_ARCH_MM_T_H
#define ARCH_X86_ARCH_MM_T_H

#ifdef CFG_X86_PLATFORM

/** @name 地址空间类型定义 */
/** @{ */
#define DEV_TYPE_NOT 0xffffffff     /**< 无效设备类型 */
#define ADRSPCE_NOT 0               /**< 无效地址空间 */
#define ADRSPCE_IO 1                /**< I/O地址空间 */
#define ADRSPCE_SDRAM 2             /**< SDRAM地址空间 */
#define ADRSPCE_RAM 3               /**< RAM地址空间 */
#define ADRSPCE_ROM 4               /**< ROM地址空间 */
#define ADRSPCE_NORFLASH 5          /**< NOR Flash地址空间 */
#define ADRSPCE_NANDFLASH 6         /**< NAND Flash地址空间 */
/** @} */

/** @name 内存映射大小位定义 */
/** @{ */
#define MAPF_SZ_BIT 0
#define MAPF_ACSZ_BIT 4
#define MAPF_SZ_16KB (1<<MAPF_SZ_BIT)
#define MAPF_SZ_32KB (2<<MAPF_SZ_BIT)
#define MAPF_SZ_4MB (8<<MAPF_SZ_BIT)

#define MAPF_ACSZ_128KB (1<<MAPF_ACSZ_BIT)
#define MAPF_ACSZ_256KB (2<<MAPF_ACSZ_BIT)
#define MAPF_ACSZ_512KB (3<<MAPF_ACSZ_BIT)
#define MAPF_ACSZ_1MB (4<<MAPF_ACSZ_BIT)
#define MAPF_ACSZ_2MB (5<<MAPF_ACSZ_BIT)
#define MAPF_ACSZ_4MB (6<<MAPF_ACSZ_BIT)
/** @} */

#define MAPONE_SIZE (0x400000)      /**< 单个映射块大小: 4MB */
#define MAP_FLAGES_VAL(RV,SALLOCSZ,MSZ) (RV|SALLOCSZ|MSZ)

/** @name 分配状态标志 */
/** @{ */
#define ADDT_EMTP_FLG 1             /**< 空闲状态 */
#define ADDT_FUEM_FLG 2             /**< 部分使用状态 */
#define ADDT_FULL_FLG 3             /**< 已满状态 */
/** @} */

/** @name 块大小定义 */
/** @{ */
#define BLK128KB_SIZE (0x20000)     /**< 128KB块大小 */
#define BLK256KB_SIZE (0x40000)     /**< 256KB块大小 */
#define BLK512KB_SIZE (0x80000)     /**< 512KB块大小 */
#define BLK1MB_SIZE (0x100000)      /**< 1MB块大小 */
#define BLK2MB_SIZE (0x200000)      /**< 2MB块大小 */
#define BLK4MB_SIZE (0x400000)      /**< 4MB块大小 */
/** @} */

/** @name 块位长度定义 */
/** @{ */
#define BLK128KB_BITL (32)
#define BLK256KB_BITL (16)
#define BLK512KB_BITL (8)
#define BLK1MB_BITL (4)
#define BLK2MB_BITL (2)
#define BLK4MB_BITL (1)
/** @} */

/** @name 块掩码定义 */
/** @{ */
#define BLK128KB_MASK (0xffffffff)
#define BLK256KB_MASK (0xffff)
#define BLK512KB_MASK (0xff)
#define BLK1MB_MASK (0xf)
#define BLK2MB_MASK (0x3)
#define BLK4MB_MASK (0x1)
/** @} */

#define BLKSZ_HEAD_MAX 6            /**< 块大小队列数量 */

/** @name 启动时地址偏移定义 */
/** @{ */
#define ETYBAK_ADR 0x2000
#define PM32_EIP_OFF (ETYBAK_ADR)
#define PM32_ESP_OFF (ETYBAK_ADR+4)
#define RM16_EIP_OFF (ETYBAK_ADR+8)
#define RM16_ESP_OFF (ETYBAK_ADR+12)

#define RWHDPACK_ADR (ETYBAK_ADR+32)
#define E80MAP_NR (ETYBAK_ADR+64)
#define E80MAP_ADRADR (ETYBAK_ADR+68)
#define E80MAP_ADR (0x5000)

#define E820MAPBASS E80MAP_ADR
#define E820MAPNUMBASS E80MAP_NR
/** @} */

/** @name VGA地址定义 */
/** @{ */
#define VGAADRS 0xa0000ULL          /**< VGA起始地址 */
#define VGAADRD 0xc0000ULL          /**< VGA结束地址 */
/** @} */

/** @name RAM类型定义(E820) */
/** @{ */
#define RAM_USABLE 1                /**< 可用RAM */
#define RAM_RESERV 2                /**< 保留RAM */
#define RAM_ACPIREC 3               /**< ACPI可回收 */
#define RAM_ACPINVS 4               /**< ACPI NVS */
#define RAM_AREACON 5               /**< 区域连续 */
/** @} */

/** @name 物理内存区域类型 */
/** @{ */
#define PMR_T_OSAPUSERRAM 1         /**< OS应用程序可用RAM */
#define PMR_T_RESERVRAM 2           /**< 保留RAM */
#define PMR_T_HWUSERRAM 8           /**< 硬件使用RAM */
#define PMR_T_ARACONRAM 0xf         /**< 区域连续RAM */
#define PMR_T_BUGRAM 0xff           /**< 错误RAM */
/** @} */

/** @name 架构标志 */
/** @{ */
#define PMR_F_X86_32 (1<<0)         /**< x86 32位 */
#define PMR_F_X86_64 (1<<1)         /**< x86 64位 */
#define PMR_F_ARM_32 (1<<2)         /**< ARM 32位 */
#define PMR_F_ARM_64 (1<<3)         /**< ARM 64位 */
#define PMR_F_ARCH_MASK 0xff        /**< 架构掩码 */
/** @} */

/**
 * @brief 物理内存区域描述符
 * 
 * 描述一块物理内存区域的属性和范围
 */
typedef struct s_PHYMMARGE {
    spinlock_t pmr_lock;            /**< 保护结构的自旋锁 */
    u32_t pmr_type;                 /**< 内存类型 (PMR_T_xxx) */
    u32_t pmr_subtype;              /**< 子类型 */
    u32_t pmr_devtype;              /**< 设备类型 */
    u32_t pmr_flags;                /**< 标志位 (PMR_F_xxx) */
    u32_t pmr_status;               /**< 当前状态 */
    u64_t pmr_start;                /**< 区域起始地址 */
    u64_t pmr_size;                 /**< 区域大小 */
    u64_t pmr_end;                  /**< 区域结束地址 */
    u64_t pmr_resv_start;           /**< 保留区起始地址 */
    u64_t pmr_resv_end;             /**< 保留区结束地址 */
    void* pmr_priv;                 /**< 私有数据指针 */
    void* pmr_ext;                  /**< 扩展数据指针 */
} phymmarge_t;

/**
 * @brief 物理地址空间描述符
 * 
 * 描述一个物理地址空间的范围和设备类型
 */
typedef struct s_PHYADRSPCE {
    u32_t ap_flags;                 /**< 标志位 */
    u32_t ap_devtype;               /**< 设备类型 */
    adr_t ap_start;                 /**< 起始地址 */
    adr_t ap_end;                   /**< 结束地址 */
} phyadrspce_t;

/**
 * @brief 内存映射描述符
 * 
 * 管理4MB大小的内存块，可分割为128KB/256KB/512KB/1MB/2MB/4MB等份
 */
typedef struct s_MMAPDSC {
    list_h_t map_list;              /**< 链表节点 */
    spinlock_t map_lock;            /**< 保护锁 */
    adr_t map_phyaddr;              /**< 物理地址起始 */
    adr_t map_phyaddr_end;          /**< 物理地址结束 */
    u32_t map_alloc_count;          /**< 已分配计数 */
    u32_t map_flags;                /**< 标志位 */
} mmapdsc_t;

/**
 * @brief 分配释放链表
 * 
 * 管理同一大小的mmapdsc队列，按状态分类(已满/部分使用/空闲)
 */
typedef struct s_ALCFRELST {
    spinlock_t afl_lock;            /**< 保护锁 */
    size_t afl_size;                /**< 块大小 */
    list_h_t afl_full_list;         /**< 已满链表 */
    list_h_t afl_empty_list;        /**< 空闲链表 */
    list_h_t afl_partial_list;      /**< 部分使用链表 */
} alcfrelst_t;

/**
 * @brief 物理内存管理器
 * 
 * 管理从128KB到4MB的多级块大小队列:
 * [0]128KB [1]256KB [2]512KB [3]1MB [4]2MB [5]4MB
 */
typedef struct s_PHYMEM {
    list_h_t pmm_list;              /**< 链表节点 */
    spinlock_t pmm_lock;            /**< 保护锁 */
    uint_t pmm_free_blocks;         /**< 空闲块数 */
    uint_t pmm_alloc_blocks;        /**< 已分配块数 */
    alcfrelst_t pmm_size_list[BLKSZ_HEAD_MAX]; /**< 按大小分类的队列数组 */
} phymem_t;

#endif // CFG_X86_PLATFORM

#endif // ARCH_X86_ARCH_MM_T_H
