/*
 * @Author: Jee Hsu
 * @Description: x86-64 MMU和页表类型定义
 *
 * 定义x86-64四级页表(PML4/PDPT/PD/PT)的数据结构
 *
 * ============================================================================
 * 页表命名约定与x86-64标准术语映射:
 * ============================================================================
 *
 *   本项目命名     | x86-64标准术语  | 说明
 *   -------------- | --------------- | -----------------------------------
 *   tdire_t        | PML4E           | Top Directory Entry (顶级目录项)
 *   sdire_t        | PDPTE           | Second Directory Entry (次级目录项)
 *   idire_t        | PDE             | Internal Directory Entry (页目录项)
 *   mdire_t        | PTE             | Middle Directory Entry (页表项)
 *   tdirearr_t     | PML4            | Top Directory Array (PML4表)
 *   sdirearr_t     | PDPT            | Second Directory Array (PDPT表)
 *   idirearr_t     | PD              | Internal Directory Array (页目录表)
 *   mdirearr_t     | PT              | Middle Directory Array (页表)
 *   mmudsc_t       | -               | MMU Descriptor (MMU描述符)
 *
 * ============================================================================
 * 虚拟地址解析(48位长模式):
 * ============================================================================
 *
 *   [47:39] PML4索引(tdire) -> [38:30] PDPT索引(sdire) ->
 *   [29:21] PD索引(idire)   -> [20:12] PT索引(mdire)   -> [11:0] 页内偏移
 *
 *   每级页表包含512个表项(9位索引)，每个表项8字节(64位)
 *   4KB页面：使用全部四级页表
 *   2MB大页：PDE.PS=1，跳过PT级
 *   1GB大页：PDPTE.PS=1，跳过PD和PT级
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_MMU_T_H
#define ARCH_X86_MMU_T_H

/** @name 页表项数量 */
/** @{ */
#define TDIRE_MAX (512)     /**< PML4表项数: 512 */
#define SDIRE_MAX (512)     /**< PDPT表项数: 512 */
#define IDIRE_MAX (512)     /**< PD表项数: 512 */
#define MDIRE_MAX (512)     /**< PT表项数: 512 */
/** @} */

/** @name 虚拟地址解析参数 */
/** @{ */
#define MDIRE_IV_RSHTBIT (12)       /**< PT索引右移位数 */
#define MDIRE_IV_BITMASK (0x1ffUL)  /**< 9位索引掩码 */
#define MSA_PADR_LSHTBIT (12)       /**< 物理地址左移位数 */
#define MDIRE_PADR_LSHTBIT (12)
#define IDIRE_IV_RSHTBIT (21)       /**< PD索引右移位数 */
#define IDIRE_IV_BITMASK (0x1ffUL)
#define IDIRE_PADR_LSHTBIT (12)
#define SDIRE_IV_RSHTBIT (30)       /**< PDPT索引右移位数 */
#define SDIRE_IV_BITMASK (0x1ffUL)
#define SDIRE_PADR_LSHTBIT (12)
#define TDIRE_IV_RSHTBIT (39)       /**< PML4索引右移位数 */
#define TDIRE_IV_BITMASK (0x1ffUL)
/** @} */

/** @name PML4表项标志位 */
/** @{ */
#define PML4E_P (1 << 0)    /**< 存在位: 1=有效 */
#define PML4E_RW (1 << 1)   /**< 读写位: 1=可写 */
#define PML4E_US (1 << 2)   /**< 用户位: 1=用户可访问 */
#define PML4E_PWT (1 << 3)  /**< 写穿透: 1=写穿透缓存 */
#define PML4E_PCD (1 << 4)  /**< 缓存禁用: 1=禁用缓存 */
/** @} */

/** @name PDPT表项标志位 */
/** @{ */
#define PDPTE_P (1 << 0)    /**< 存在位 */
#define PDPTE_RW (1 << 1)   /**< 读写位 */
#define PDPTE_US (1 << 2)   /**< 用户位 */
#define PDPTE_PWT (1 << 3)  /**< 写穿透 */
#define PDPTE_PCD (1 << 4)  /**< 缓存禁用 */
#define PDPTE_PS (1 << 7)   /**< 页大小: 1=1GB大页 */
/** @} */

/** @name PD表项标志位 */
/** @{ */
#define PDTE_P (1 << 0)     /**< 存在位 */
#define PDTE_RW (1 << 1)    /**< 读写位 */
#define PDTE_US (1 << 2)    /**< 用户位 */
#define PDTE_PWT (1 << 3)   /**< 写穿透 */
#define PDTE_PCD (1 << 4)   /**< 缓存禁用 */
#define PDTE_PS (1 << 7)    /**< 页大小: 1=2MB大页 */
/** @} */

/** @name PT表项标志位 */
/** @{ */
#define PTE_P (1 << 0)      /**< 存在位 */
#define PTE_RW (1 << 1)     /**< 读写位 */
#define PTE_US (1 << 2)     /**< 用户位 */
#define PTE_PWT (1 << 3)    /**< 写穿透 */
#define PTE_PCD (1 << 4)    /**< 缓存禁用 */
#define PTE_PAT (1 << 7)    /**< 页属性表索引 */
/** @} */

#define PDPTEPHYADR_IP_LSHTBIT (12)
#define PDTEPHYADR_IP_LSHTBIT (12)
#define PTEPHYADR_IP_LSHTBIT (12)
#define PFMPHYADR_IP_LSHTBIT (12)
#define PTE_HAVE_MASK (~0xfff)      /**< 提取物理地址掩码 */
#define PDE_HAVE_MASK (~0xfff)
#define PDPTE_HAVE_MASK (~0xfff)
#define PML4E_HAVE_MASK (~0xfff)

/**
 * @brief PT表项标志位域结构(4KB页)
 * 
 * 页表(Page Table)的每个表项指向一个4KB物理页
 */
typedef struct MDIREFLAGS {
    u64_t m_p : 1;      /**< [0] 存在位: 1=映射有效 */
    u64_t m_rw : 1;     /**< [1] 读写位: 1=可写, 0=只读 */
    u64_t m_us : 1;     /**< [2] 用户位: 1=用户可访问 */
    u64_t m_pwt : 1;    /**< [3] 写穿透: 1=启用写穿透 */
    u64_t m_pcd : 1;    /**< [4] 缓存禁用: 1=禁用缓存 */
    u64_t m_a : 1;      /**< [5] 访问位: CPU访问后置1 */
    u64_t m_d : 1;      /**< [6] 脏位: CPU写入后置1 */
    u64_t m_pat : 1;    /**< [7] PAT索引位 */
    u64_t m_g : 1;      /**< [8] 全局位: 1=TLB不刷新 */
    u64_t m_ig1 : 3;    /**< [9:11] 保留给OS使用 */
    u64_t m_msa : 40;   /**< [12:51] 物理页帧号(右移12位) */
    u64_t m_ig2 : 11;   /**< [52:62] 保留给OS使用 */
    u64_t m_xd : 1;     /**< [63] 执行禁用: 1=禁止执行 */
} __attribute__((packed)) mdireflags_t;

/**
 * @brief PT表项联合体
 */
typedef struct MDIRE {
    union {
        mdireflags_t m_flags;   /**< 位域访问 */
        u64_t m_entry;          /**< 整体访问 */
    } __attribute__((packed));
} __attribute__((packed)) mdire_t;

/**
 * @brief PD表项标志位域结构
 * 
 * 页目录(Page Directory)的每个表项指向一个页表
 * 或直接映射2MB大页(当PS=1时)
 */
typedef struct IDIREFLAGS {
    u64_t i_p : 1;      /**< [0] 存在位 */
    u64_t i_rw : 1;     /**< [1] 读写位 */
    u64_t i_us : 1;     /**< [2] 用户位 */
    u64_t i_pwt : 1;    /**< [3] 写穿透 */
    u64_t i_pcd : 1;    /**< [4] 缓存禁用 */
    u64_t i_a : 1;      /**< [5] 访问位 */
    u64_t i_ig1 : 1;    /**< [6] 保留 */
    u64_t i_ps : 1;     /**< [7] 页大小: 0=4KB页表, 1=2MB大页 */
    u64_t i_ig2 : 4;    /**< [8:11] 保留 */
    u64_t i_mdir : 40;  /**< [12:51] 页表物理地址 */
    u64_t i_ig3 : 11;   /**< [52:62] 保留 */
    u64_t i_xd : 1;     /**< [63] 执行禁用 */
} __attribute__((packed)) idireflags_t;

/**
 * @brief PD表项联合体
 */
typedef struct IDIRE {
    union {
        idireflags_t i_flags;
        u64_t i_entry;
    } __attribute__((packed));
} __attribute__((packed)) idire_t;

/**
 * @brief PDPT表项标志位域结构
 * 
 * 页目录指针表(PDPT)的每个表项指向一个页目录
 * 或直接映射1GB大页(当PS=1时)
 */
typedef struct SDIREFLAGS {
    u64_t s_p : 1;      /**< [0] 存在位 */
    u64_t s_rw : 1;     /**< [1] 读写位 */
    u64_t s_us : 1;     /**< [2] 用户位 */
    u64_t s_pwt : 1;    /**< [3] 写穿透 */
    u64_t s_pcd : 1;    /**< [4] 缓存禁用 */
    u64_t s_a : 1;      /**< [5] 访问位 */
    u64_t s_ig1 : 1;    /**< [6] 保留 */
    u64_t s_ps : 1;     /**< [7] 页大小: 0=4KB, 1=1GB大页 */
    u64_t s_ig2 : 4;    /**< [8:11] 保留 */
    u64_t s_idir : 40;  /**< [12:51] 页目录物理地址 */
    u64_t s_ig3 : 11;   /**< [52:62] 保留 */
    u64_t s_xd : 1;     /**< [63] 执行禁用 */
} __attribute__((packed)) sdireflags_t;

/**
 * @brief PDPT表项联合体
 */
typedef struct SDIRE {
    union {
        sdireflags_t s_flags;
        u64_t s_entry;
    } __attribute__((packed));
} __attribute__((packed)) sdire_t;

/**
 * @brief PML4表项标志位域结构
 * 
 * 顶级页表(PML4)的每个表项指向一个PDPT
 */
typedef struct TDIREFLAGS {
    u64_t t_p : 1;      /**< [0] 存在位 */
    u64_t t_rw : 1;     /**< [1] 读写位 */
    u64_t t_us : 1;     /**< [2] 用户位 */
    u64_t t_pwt : 1;    /**< [3] 写穿透 */
    u64_t t_pcd : 1;    /**< [4] 缓存禁用 */
    u64_t t_a : 1;      /**< [5] 访问位 */
    u64_t t_ig1 : 1;    /**< [6] 保留 */
    u64_t t_rv1 : 1;    /**< [7] 保留(必须为0) */
    u64_t t_ig2 : 4;    /**< [8:11] 保留 */
    u64_t t_sdir : 40;  /**< [12:51] PDPT物理地址 */
    u64_t t_ig3 : 11;   /**< [52:62] 保留 */
    u64_t t_xd : 1;     /**< [63] 执行禁用 */
} __attribute__((packed)) tdireflags_t;

/**
 * @brief PML4表项联合体
 */
typedef struct TDIRE {
    union {
        tdireflags_t t_flags;
        u64_t t_entry;
    } __attribute__((packed));
} __attribute__((packed)) tdire_t;

/** @brief PT页表数组(512项) */
typedef struct MDIREARR {
    mdire_t mde_arr[MDIRE_MAX];
} __attribute__((packed)) mdirearr_t;

/** @brief PD页目录数组(512项) */
typedef struct IDIREARR {
    idire_t ide_arr[IDIRE_MAX];
} __attribute__((packed)) idirearr_t;

/** @brief PDPT页目录指针数组(512项) */
typedef struct SDIREARR {
    sdire_t sde_arr[SDIRE_MAX];
} __attribute__((packed)) sdirearr_t;

/** @brief PML4顶级页表数组(512项) */
typedef struct TDIREARR {
    tdire_t tde_arr[TDIRE_MAX];
} __attribute__((packed)) tdirearr_t;

/**
 * @brief CR3寄存器标志位域结构
 * 
 * CR3寄存器保存PML4表的物理基地址
 */
typedef struct CR3SFLGS {
    u64_t c3s_pcid : 12;    /**< [0:11] 进程上下文ID(可选) */
    u64_t c3s_plm4a : 40;   /**< [12:51] PML4表物理地址 */
    u64_t c3s_rv : 11;      /**< [52:62] 保留 */
    u64_t c3s_tbc : 1;      /**< [63] TLB刷新控制 */
} __attribute__((packed)) cr3sflgs_t;

/**
 * @brief CR3寄存器联合体
 */
typedef struct CR3S {
    union {
        cr3sflgs_t c3s_c3sflgs;
        u64_t c3s_entry;
    } __attribute__((packed));
} __attribute__((packed)) cr3s_t;

/**
 * @brief MMU描述符结构
 * 
 * 每个进程拥有一个MMU描述符，管理其虚拟地址空间
 */
typedef struct MMUDSC {
    spinlock_t mud_lock;        /**< 保护MMU描述符的自旋锁 */
    u64_t mud_status;             /**< MMU状态 */
    u64_t mud_flag;             /**< MMU标志 */
    tdirearr_t *mud_tdirearr;   /**< PML4表指针 */
    cr3s_t mud_cr3;             /**< CR3寄存器值 */
    list_h_t mud_tdirhead;      /**< PML4表页链表 */
    list_h_t mud_sdirhead;      /**< PDPT表页链表 */
    list_h_t mud_idirhead;      /**< PD表页链表 */
    list_h_t mud_mdirhead;      /**< PT表页链表 */
    uint_t mud_tdirmsanr;       /**< PML4表页数量 */
    uint_t mud_sdirmsanr;       /**< PDPT表页数量 */
    uint_t mud_idirmsanr;       /**< PD表页数量 */
    uint_t mud_mdirmsanr;       /**< PT表页数量 */
} mmudsc_t;

#endif // ARCH_X86_MMU_T_H
