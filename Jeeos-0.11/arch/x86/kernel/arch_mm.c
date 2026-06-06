/*
 * @Author: Jee Hsu
 * @Description: 架构层内存管理 - 物理内存探测和初始化
 *
 * 负责从BIOS E820内存映射构建物理内存区域结构:
 * 1. 解析e820map_t数组(BIOS提供)
 * 2. 创建phymmarge_t数组(物理内存区域描述)
 * 3. 按地址排序，初始化内存管理器
 *
 * 内存区域类型:
 * - RAM_USABLE:  可用内存(操作系统可分配)
 * - RAM_RESERV:  保留内存(BIOS/硬件使用)
 * - RAM_ACPIREC: ACPI可回收内存
 * - RAM_ACPINVS: ACPI NVS内存
 * - RAM_AREACON: 有缺陷内存
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

/**
 * 初始化物理内存区域结构
 * @param initp 要初始化的结构指针
 */
void phymmarge_init(phymmarge_t *initp) {
    if (NULL == initp) {
        return;
    }
    arch_spinlock_init(&initp->pmr_lock);
    initp->pmr_type = 0;        /* 内存类型 */
    initp->pmr_subtype = 0;       /* 子类型 */
    initp->pmr_devtype = 0;
    initp->pmr_flags = 0;        /* 标志 */
    initp->pmr_status = 0;        /* 状态 */
    initp->pmr_start = 0;       /* 起始地址 */
    initp->pmr_size = 0;       /* 大小 */
    initp->pmr_end = 0;         /* 结束地址 */
    initp->pmr_resv_start = 0;
    initp->pmr_resv_end = 0;
    initp->pmr_priv = NULL;
    initp->pmr_ext = NULL;
    return;
}

/**
 * 根据启动参数计算物理内存区域数组的地址和大小
 * @param boot_param 启动参数结构 (mabp: machine/arch boot parameter)
 * @param ret_phymem_vaddr 返回区域数组地址
 * @param ret_phymem_size 返回区域数组大小
 */
void ret_phymmarge_adrandsz(abootparam_t *boot_param, phymmarge_t **ret_phymem_vaddr, u64_t *ret_phymem_size) {
    if (NULL == boot_param || 0 == boot_param->mb_e820sz || NULL == boot_param->mb_e820padr || 0 == boot_param->mb_e820nr) {
        *ret_phymem_size = 0;
        *ret_phymem_vaddr = NULL;
        return;
    }
    /* 计算需要的空间: e820条目数 * phymmarge_t结构大小 */
    u64_t tmpsz = boot_param->mb_e820nr * sizeof(phymmarge_t);
    u64_t tmpphyadr = boot_param->mb_nextwtpadr;
    if (0 != initchkadr_is_ok(boot_param, tmpphyadr, tmpsz)) {
        *ret_phymem_size = 0;
        *ret_phymem_vaddr = NULL;
        return;
    }
    *ret_phymem_size = tmpsz;
    *ret_phymem_vaddr = (phymmarge_t *)((adr_t)tmpphyadr);
    return;
}

/**
 * 根据单个e820条目初始化物理内存区域结构
 * @param e820_entry E820条目指针
 * @param phymem_range 物理内存区域结构指针
 * @return TRUE成功, FALSE失败
 */
bool_t init_one_pmrge(e820map_t *e820_entry, phymmarge_t *phymem_range) {
    u32_t ptype = 0, pstype = 0;
    if (NULL == e820_entry || NULL == phymem_range) {
        return FALSE;
    }
    phymmarge_init(phymem_range);
    
    /* 根据E820类型映射到内部类型 */
    switch (e820_entry->type) {
        case RAM_USABLE:    /* 可用RAM -> 操作系统可用 */
            ptype = PMR_T_OSAPUSERRAM;
            pstype = RAM_USABLE;
            break;
        case RAM_RESERV:    /* 保留RAM -> 系统保留 */
            ptype = PMR_T_RESERVRAM;
            pstype = RAM_RESERV;
            break;
        case RAM_ACPIREC:   /* ACPI可回收 -> 硬件使用 */
            ptype = PMR_T_HWUSERRAM;
            pstype = RAM_ACPIREC;
            break;
        case RAM_ACPINVS:   /* ACPI NVS -> 硬件使用 */
            ptype = PMR_T_HWUSERRAM;
            pstype = RAM_ACPINVS;
            break;
        case RAM_AREACON:   /* 有缺陷 -> 标记为BUG */
            ptype = PMR_T_BUGRAM;
            pstype = RAM_AREACON;
            break;
        default:
            break;
    }
    if (0 == ptype) {
        return FALSE;
    }
    /* 填充结构字段 */
    phymem_range->pmr_type = ptype;
    phymem_range->pmr_subtype = pstype;
    phymem_range->pmr_flags = PMR_F_X86_64;
    phymem_range->pmr_start = e820_entry->saddr;
    phymem_range->pmr_size = e820_entry->lsize;
    phymem_range->pmr_end = e820_entry->saddr + e820_entry->lsize - 1;
    return TRUE;
}

/**
 * 交换两个物理内存区域结构(用于排序)
 * @param src 源结构指针
 * @param dst 目标结构指针
 */
void phymmarge_swap(phymmarge_t *src, phymmarge_t *dst) {
    phymmarge_t tmp;
    phymmarge_init(&tmp);
    memcopy(src, &tmp, sizeof(phymmarge_t));
    memcopy(dst, src, sizeof(phymmarge_t));
    memcopy(&tmp, dst, sizeof(phymmarge_t));
    return;
}

/**
 * 对物理内存区域数组按起始地址升序排序(冒泡排序)
 * @param phymem_array 区域数组指针
 * @param count 数组元素个数
 */
void phymmarge_sort(phymmarge_t *phymem_array, u64_t count) {
    u64_t i, j, k = count - 1;
    for (j = 0; j < k; j++) {
        for (i = 0; i < k - j; i++) {
            if (phymem_array[i].pmr_start > phymem_array[i + 1].pmr_start) {
                phymmarge_swap(&phymem_array[i], &phymem_array[i + 1]);
            }
        }
    }
    return;
}

/**
 * 物理内存区域初始化核心函数
 * 遍历E820数组，为每个条目创建对应的phymmarge_t结构
 * @param e820_array E820数组指针
 * @param e820_count E820条目数
 * @param phymem_array 目标phymmarge_t数组指针
 * @return 成功初始化的区域数量
 */
u64_t initpmrge_core(e820map_t *e820_array, u64_t e820_count, phymmarge_t *phymem_array) {
    u64_t retnr = 0;
    if (NULL == e820_array || NULL == phymem_array || e820_count < 1) {
        return 0;
    }
    for (u64_t i = 0; i < e820_count; i++) {
        /* 根据一个e820map_t结构建立一个phymmarge_t结构 */
        if (init_one_pmrge(&e820_array[i], &phymem_array[i]) == FALSE) {
            return retnr;
        }
        retnr++;
    }
    return retnr;
}

/**
 * 初始化物理内存区域
 * 
 * 主要工作:
 * 1. 从启动参数获取E820数组
 * 2. 分配phymmarge_t数组空间
 * 3. 遍历E820创建phymmarge_t结构
 * 4. 按地址排序
 * 5. 更新启动参数中的内存信息
 */
void init_phymmarge() {
    abootparam_t *mabp = &kabootparam;
    phymmarge_t *pmarge_adr = NULL;
    u64_t pmrgesz = 0;
    
    /* 计算phymmarge_t数组的地址和大小 */
    ret_phymmarge_adrandsz(mabp, &pmarge_adr, &pmrgesz);
    if (NULL == pmarge_adr || 0 == pmrgesz) {
        system_error("init_phymmarge->NULL==pmarge_adr||0==pmrgesz\n");
        return;
    }
    
    u64_t tmppmrphyadr = mabp->mb_nextwtpadr;
    if ((adr_t)tmppmrphyadr != ((adr_t)pmarge_adr)) {
        system_error("init_phymmarge->tmppmrphyadr!=pmarge_adr2phyadr\n");
        return;
    }
    
    /* 获取E820数组指针 */
    e820map_t *e8p = (e820map_t *)((adr_t)(mabp->mb_e820padr));
    
    /* 建立phymmarge_t结构数组 */
    u64_t ipmgnr = initpmrge_core(e8p, mabp->mb_e820nr, pmarge_adr);
    if (ipmgnr == 0) {
        system_error("init_phymmarge->initpmrge_core ret 0\n");
        return;
    }
    if ((ipmgnr * sizeof(phymmarge_t)) != pmrgesz) {
        system_error("init_phymmarge->ipmgnr*sizeof(phymmarge_t))!=pmrgesz\n");
        return;
    }
    
    /* 把phymmarge_t数组信息保存到启动参数结构 */
    mabp->mb_e820expadr = tmppmrphyadr;
    mabp->mb_e820exnr = ipmgnr;
    mabp->mb_e820exsz = ipmgnr * sizeof(phymmarge_t);
    mabp->mb_nextwtpadr = PAGE_ALIGN(mabp->mb_e820expadr + mabp->mb_e820exsz);
    
    /* 按地址从低到高排序 */
    phymmarge_sort(pmarge_adr, ipmgnr);
    return;
}

/**
 * 架构层内存初始化入口
 * 
 * 调用顺序:
 * 1. init_phymmarge() - 扫描物理内存
 * 2. init_memmgr()    - 初始化内存管理器
 */
void init_archmm() {
    init_phymmarge();
    init_memmgr();
    printk("[MM] %dMB RAM, %d pages free.\n", 
           (memmgrob.mo_freepages * 4) / 1024, memmgrob.mo_freepages);
    return;
}
