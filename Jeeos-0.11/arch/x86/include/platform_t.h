/*
 * @Author: Jee Hsu
 * @Description: 架构相关 - platform_t
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_PLATFORM_T_H
#define ARCH_X86_PLATFORM_T_H
#include "video_t.h"

#ifdef CFG_X86_PLATFORM
#define CPUCORE_MAX 1
#define SDRAM_MAPVECTPHY_ADDR 0x30000000

#define KRNL_INRAM_START 0x30000000
#define LINK_VIRT_ADDR 0x30008000
#define LINK_LINE_ADDR 0x30008000
#define KERNEL_VIRT_ADDR 0x30008000
#define KERNEL_PHYI_ADDR 0x30008000
#define PAGE_TLB_DIR 0x30004000
#define PAGE_TLB_SIZE 4096
#define INIT_HEAD_STACK_ADDR 0x34000000

#define CPU_VECTOR_PHYADR 0x30000000
#define CPU_VECTOR_VIRADR 0

#define PTE_SECT_AP (3<<10)
#define PTE_SECT_DOMAIN (0<<5)
#define PTE_SECT_NOCW (0<<2)
#define PTE_SECT_BIT (2)

#define PLFM_ADRSPCE_NR 29

#define INTSRC_MAX 32

#define KRNL_MAP_VIRTADDRESS_SIZE 0x400000000
#define KRNL_VIRTUAL_ADDRESS_START 0xffff800000000000
#define KRNL_VIRTUAL_ADDRESS_END 0xffffffffffffffff
#define USER_VIRTUAL_ADDRESS_START 0
#define USER_VIRTUAL_ADDRESS_END 0x00007fffffffffff
#define KRNL_MAP_PHYADDRESS_START 0
#define KRNL_MAP_PHYADDRESS_END 0x400000000
#define KRNL_MAP_PHYADDRESS_SIZE 0x400000000
#define KRNL_MAP_VIRTADDRESS_START KRNL_VIRTUAL_ADDRESS_START
#define KRNL_MAP_VIRTADDRESS_END (KRNL_MAP_VIRTADDRESS_START+KRNL_MAP_VIRTADDRESS_SIZE)
#define KRNL_ADDR_ERROR 0xf800000000000

#define MBS_MIGC (u64_t)((((u64_t)'L')<<56)|(((u64_t)'M')<<48)|(((u64_t)'O')<<40)|(((u64_t)'S')<<32)|(((u64_t)'M')<<24)|(((u64_t)'B')<<16)|(((u64_t)'S')<<8)|((u64_t)'P'))

/**
 * @brief ACPI RSDP结构(根系统描述指针)
 * 
 * ACPI表的入口点，用于定位ACPI配置表
 */
typedef struct s_MRSDP {
    u64_t rp_sign;          /**< 签名"RSD PTR " */
    u8_t rp_chksum;         /**< 校验和(前20字节) */
    u8_t rp_oemid[6];       /**< OEM标识符 */
    u8_t rp_revn;           /**< ACPI版本: 0=1.0, 2=2.0+ */
    u32_t rp_rsdtphyadr;    /**< RSDT物理地址(32位) */
    u32_t rp_len;           /**< 结构长度 */
    u64_t rp_xsdtphyadr;    /**< XSDT物理地址(64位) */
    u8_t rp_echksum;        /**< 扩展校验和 */
    u8_t rp_resv[3];        /**< 保留 */
} __attribute__((packed)) mrsdp_t;

/**
 * @brief 引导参数结构
 * 
 * 由bootloader填充，传递给内核的启动信息
 * 包含内存布局、显示参数、ACPI信息等
 */
typedef struct s_ABOOTPARAM {
    u64_t mb_migc;          /**< 魔数: "LMOSMBSP" */
    u64_t mb_chksum;        /**< 参数校验和 */
    u64_t mb_krlinitstack;  /**< 内核初始栈地址 */
    u64_t mb_krlitstacksz;  /**< 内核初始栈大小 */
    u64_t mb_imgpadr;       /**< 系统映像物理地址 */
    u64_t mb_imgsz;         /**< 系统映像大小 */
    u64_t mb_krlimgpadr;    /**< 内核映像物理地址 */
    u64_t mb_krlsz;         /**< 内核大小 */
    u64_t mb_krlvec;        /**< 内核入口向量 */
    u64_t mb_krlrunmode;    /**< 内核运行模式 */
    u64_t mb_kalldendpadr;  /**< 内核结束物理地址 */
    u64_t mb_ksepadrs;      /**< 内核分隔段起始 */
    u64_t mb_ksepadre;      /**< 内核分隔段结束 */
    u64_t mb_kservadrs;     /**< 内核服务段起始 */
    u64_t mb_kservadre;     /**< 内核服务段结束 */
    u64_t mb_nextwtpadr;    /**< 下一等待地址 */
    u64_t mb_bfontpadr;     /**< 位图字体物理地址 */
    u64_t mb_bfontsz;       /**< 位图字体大小 */
    u64_t mb_fvrmphyadr;    /**< 帧缓冲物理地址 */
    u64_t mb_fvrmsz;        /**< 帧缓冲大小 */
    u64_t mb_cpumode;       /**< CPU模式(32/64位) */
    u64_t mb_memsz;         /**< 物理内存总大小 */
    u64_t mb_e820padr;      /**< E820内存映射物理地址 */
    u64_t mb_e820nr;        /**< E820条目数量 */
    u64_t mb_e820sz;        /**< E820数据大小 */
    u64_t mb_e820expadr;    /**< E820扩展地址 */
    u64_t mb_e820exnr;      /**< E820扩展条目数 */
    u64_t mb_e820exsz;      /**< E820扩展大小 */
    u64_t mb_memznpadr;     /**< 内存区域物理地址 */
    u64_t mb_memznnr;       /**< 内存区域数量 */
    u64_t mb_memznsz;       /**< 内存区域大小 */
    u64_t mb_memznchksum;   /**< 内存区域校验和 */
    u64_t mb_memmappadr;    /**< 内存映射物理地址 */
    u64_t mb_memmapnr;      /**< 内存映射数量 */
    u64_t mb_memmapsz;      /**< 内存映射大小 */
    u64_t mb_memmapchksum;  /**< 内存映射校验和 */
    u64_t mb_pml4padr;      /**< PML4页表物理地址 */
    u64_t mb_subpageslen;   /**< 子页表长度 */
    u64_t mb_kpmapphymemsz; /**< 内核物理映射大小 */
    u64_t mb_ebdaphyadr;    /**< EBDA物理地址 */
    mrsdp_t mb_mrsdp;       /**< ACPI RSDP结构 */
    graph_t mb_ghparm;      /**< 图形参数 */
} __attribute__((packed)) abootparam_t;

#define MBSPADR ((abootparam_t*)(0x100000))

#define BFH_RW_R 1
#define BFH_RW_W 2

#define BFH_BUF_SZ 0x1000
#define BFH_ONERW_SZ 0x1000
#define BFH_RWONE_OK 1
#define BFH_RWONE_ER 2
#define BFH_RWALL_OK 3

#define FHDSC_NMAX 192
#define FHDSC_SZMAX 256
#define MDC_ENDGIC 0xaaffaaffaaffaaff
#define MDC_RVGIC 0xffaaffaaffaaffaa

#define MLOSDSC_OFF (0x1000)
#define RAM_USABLE 1
#define RAM_RESERV 2
#define RAM_ACPIREC 3
#define RAM_ACPINVS 4
#define RAM_AREACON 5

/**
 * @brief E820内存映射条目结构
 * 
 * BIOS通过E820调用返回的内存区域信息
 * 描述物理内存的分布和类型
 */
typedef struct s_e820 {
    u64_t saddr;            /**< 内存区域起始地址 */
    u64_t lsize;            /**< 内存区域大小 */
    u32_t type;             /**< 区域类型: RAM_USABLE/RESERV/ACPI */
} __attribute__((packed)) e820map_t;

/**
 * @brief 文件头描述符结构
 * 
 * 描述内核映像中的一个文件/模块
 */
typedef struct s_fhdsc {
    u64_t fhd_type;         /**< 文件类型 */
    u64_t fhd_subtype;      /**< 文件子类型 */
    u64_t fhd_status;       /**< 文件状态 */
    u64_t fhd_id;           /**< 文件ID */
    u64_t fhd_intsfsoff;    /**< 在映像中的起始偏移 */
    u64_t fhd_intsfend;     /**< 在映像中的结束偏移 */
    u64_t fhd_frealsz;      /**< 文件实际大小 */
    u64_t fhd_fsum;         /**< 文件校验和 */
    char fhd_name[FHDSC_NMAX]; /**< 文件名 */
} fhdsc_t;

/**
 * @brief 系统映像描述符结构
 * 
 * 描述整个系统映像的布局和校验信息
 */
typedef struct s_mlosrddsc {
    u64_t mdc_mgic;         /**< 魔数 */
    u64_t mdc_sfsum;        /**< 系统文件校验和 */
    u64_t mdc_sfsoff;       /**< 系统文件起始偏移 */
    u64_t mdc_sfeoff;       /**< 系统文件结束偏移 */
    u64_t mdc_sfrlsz;       /**< 系统文件实际大小 */
    u64_t mdc_ldrbk_s;      /**< 加载器块起始 */
    u64_t mdc_ldrbk_e;      /**< 加载器块结束 */
    u64_t mdc_ldrbk_rsz;    /**< 加载器块大小 */
    u64_t mdc_ldrbk_sum;    /**< 加载器块校验和 */
    u64_t mdc_fhdbk_s;      /**< 文件头块起始 */
    u64_t mdc_fhdbk_e;      /**< 文件头块结束 */
    u64_t mdc_fhdbk_rsz;    /**< 文件头块大小 */
    u64_t mdc_fhdbk_sum;    /**< 文件头块校验和 */
    u64_t mdc_filbk_s;      /**< 文件块起始 */
    u64_t mdc_filbk_e;      /**< 文件块结束 */
    u64_t mdc_filbk_rsz;    /**< 文件块大小 */
    u64_t mdc_filbk_sum;    /**< 文件块校验和 */
    u64_t mdc_ldrcodenr;    /**< 加载器代码数量 */
    u64_t mdc_fhdnr;        /**< 文件头数量 */
    u64_t mdc_filnr;        /**< 文件数量 */
    u64_t mdc_endgic;       /**< 结束魔数 */
    u64_t mdc_rv;           /**< 保留 */
} mlosrddsc_t;

#endif // CFG_X86_PLATFORM

#endif // ARCH_X86_PLATFORM_T_H
