/*
 * @Author: Jee Hsu
 * @Description: 内存管理初始化模块
 *
 * 负责内核内存子系统的初始化：
 * - 页表设置(PML4/PDPT/PD/PT四级页表)
 * - 物理内存区域初始化
 * - 页描述符和内存区域初始化
 * - SLAB分配器初始化
 *
 * 初始化顺序(不可更改)：
 * 1. copy_pages_data() - 建立新的内核页表
 * 2. copy_fvm_data() - 复制帧缓冲区映射
 * 3. init_mmaerge() - 初始化物理内存区域
 * 4. init_msadsc() - 初始化页描述符
 * 5. init_mmarea() - 初始化内存区域管理
 * 6. init_slab() - 初始化SLAB分配器
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

void memmgrob_init(memmgrob_t *initp) {
    list_init(&initp->mo_list);
    spinlock_init(&initp->mo_lock);
    initp->mo_status = 0;
    initp->mo_flags = 0;
    initp->mo_memsz = 0;
    initp->mo_maxpages = 0;
    initp->mo_freepages = 0;
    initp->mo_alocpages = 0;
    initp->mo_resvpages = 0;
    initp->mo_horizline = 0;
    initp->mo_pmagestat = NULL;
    initp->mo_pmagenr = 0;
    initp->mo_msadscstat = NULL;
    initp->mo_msanr = 0;
    initp->mo_mareastat = NULL;
    initp->mo_mareanr = 0;
    initp->mo_privp = NULL;
    initp->mo_extp = NULL;
    return;
}

bool_t copy_pages_data(abootparam_t *mabp) {
    uint_t topgadr = mabp->mb_nextwtpadr;
    if (initchkadr_is_ok(mabp, topgadr, mabp->mb_subpageslen) != 0) {
        return FALSE;
    }

    uint_t *p = (uint_t *)phyadr_to_viradr((adr_t)topgadr);
    uint_t *pdpte = (uint_t *)(((uint_t)p) + 0x1000);
    uint_t *pde = (uint_t *)(((uint_t)p) + 0x2000);
    for (uint_t mi = 0; mi < PGENTY_SIZE; mi++) {
        p[mi] = 0;
        pdpte[mi] = 0;
    }
    uint_t adr = 0;
    uint_t pdepd = 0;
    for (uint_t pdei = 0; pdei < 16; pdei++) {
        pdepd = (uint_t)viradr_to_phyadr((adr_t)pde);
        pdpte[pdei] = (uint_t)(pdepd | KPDPTE_RW | KPDPTE_P);
        for (uint_t pdeii = 0; pdeii < PGENTY_SIZE; pdeii++) {
            pde[pdeii] = 0 | adr | KPDE_PS | KPDE_RW | KPDE_P;
            adr += 0x200000;
        }
        pde = (uint_t *)((uint_t)pde + 0x1000);
    }
    uint_t pdptepd = (uint_t)viradr_to_phyadr((adr_t)pdpte);
    p[((KRNL_VIRTUAL_ADDRESS_START) >> KPML4_SHIFT) & 0x1ff] = (uint_t)(pdptepd | KPML4_RW | KPML4_P);

    mabp->mb_pml4padr = topgadr;
    mabp->mb_subpageslen = (uint_t)(0x1000 * 16 + 0x2000);
    mabp->mb_kpmapphymemsz = (uint_t)(0x400000000);
    mabp->mb_nextwtpadr = PAGE_ALIGN(mabp->mb_pml4padr + mabp->mb_subpageslen);
    return TRUE;
}

bool_t copy_fvm_data(abootparam_t *mabp, dftgraph_t *dgp) {
    u64_t tofvadr = mabp->mb_nextwtpadr;
    if (initchkadr_is_ok(mabp, tofvadr, dgp->gh_fvrmsz) != 0) {
        return FALSE;
    }
    sint_t retcl = m2mcopy((void *)((uint_t)dgp->gh_fvrmphyadr), (void *)phyadr_to_viradr((adr_t)(tofvadr)), (sint_t)dgp->gh_fvrmsz);
    if (retcl != (sint_t)dgp->gh_fvrmsz) {
        return FALSE;
    }
    dgp->gh_fvrmphyadr = phyadr_to_viradr((adr_t)tofvadr);
    mabp->mb_fvrmphyadr = tofvadr;
    mabp->mb_nextwtpadr = PAGE_ALIGN(tofvadr + dgp->gh_fvrmsz);
    return TRUE;
}

void memi_set_mmutabl(uint_t tblpadr, void *edatap) {
    set_cr3(tblpadr);
    return;
}

void init_copy_pagesfvm() {
    if (copy_pages_data(&kabootparam) == FALSE) {
        system_error("copy_pages_data fail");
    }
    if (copy_fvm_data(&kabootparam, &kdftgh) == FALSE) {
        system_error("copy_fvm_data fail");
    }
    memi_set_mmutabl(kabootparam.mb_pml4padr, NULL);
    return;
}

// 完整版内存管理初始化
void init_memmgr() {
    abootparam_t *mabp = &kabootparam;
    
    // 1. 初始化页表
    init_copy_pagesfvm();
    
    // 2. 初始化内存管理对象
    memmgrob_init(&memmgrob);
    
    // 3. 初始化页描述符数组
    init_msadsc();
    
    // 4. 标记内核占用的页面
    init_search_krloccupymm(mabp);
    
    // 5. 初始化内存区域
    init_mmarea();
    
    // 6. 初始化伙伴系统（页面合并）
    init_merlove_mem();
    
    // 7. 更新内存管理对象中的页面计数
    mmarea_t *marea = (mmarea_t *)phyadr_to_viradr((adr_t)mabp->mb_memznpadr);
    for (uint_t i = 0; i < mabp->mb_memznnr; i++) {
        memmgrob.mo_maxpages += marea[i].ma_maxpages;
        memmgrob.mo_freepages += marea[i].ma_freepages;
    }
    memmgrob.mo_mareastat = marea;
    memmgrob.mo_mareanr = mabp->mb_memznnr;
    memmgrob.mo_msadscstat = (msadsc_t *)phyadr_to_viradr((adr_t)mabp->mb_memmappadr);
    memmgrob.mo_msanr = mabp->mb_memmapnr;
    
    /* 内存管理器初始化完成 */
    return;
}

// 简化版内存管理信息显示
void disp_memmgrob() {
    printk("Memory manager object (simplified)\n");
    return;
}
