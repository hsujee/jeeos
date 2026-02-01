/*
 * @Author: Jee Hsu
 * @Description: MMU页表管理 - 虚拟地址映射
 *
 * 实现x86-64四级页表管理：
 * - PML4 (Page Map Level 4) - 顶级页目录 [本项目: tdire/tdirearr]
 * - PDPT (Page Directory Pointer Table) - 页目录指针表 [本项目: sdire/sdirearr]
 * - PD (Page Directory) - 页目录 [本项目: idire/idirearr]
 * - PT (Page Table) - 页表 [本项目: mdire/mdirearr]
 *
 * 详细命名约定请参见 mmu_t.h 文件头部注释
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_MMU_H
#define ARCH_X86_MMU_H

/**
 * @brief 初始化MMU描述符
 * @param init MMU描述符指针
 */
void mmudsc_init(mmudsc_t* init);

/**
 * @brief 分配顶级页目录数组
 * @param mmulocked MMU描述符指针
 * @return 分配的物理页描述符
 */
msadsc_t* mmu_new_tdirearr(mmudsc_t* mmulocked);

/**
 * @brief 释放顶级页目录数组
 * @param mmulocked MMU描述符指针
 * @param tdirearr 顶级页目录数组
 * @param msa 物理页描述符
 * @return TRUE成功，FALSE失败
 */
bool_t mmu_del_tdirearr(mmudsc_t* mmulocked, tdirearr_t* tdirearr, msadsc_t* msa);

/**
 * @brief 分配二级页目录数组
 */
msadsc_t* mmu_new_sdirearr(mmudsc_t* mmulocked);

/**
 * @brief 释放二级页目录数组
 */
bool_t mmu_del_sdirearr(mmudsc_t* mmulocked, sdirearr_t* sdirearr, msadsc_t* msa);

/**
 * @brief 分配三级页目录数组
 */
msadsc_t* mmu_new_idirearr(mmudsc_t* mmulocked);

/**
 * @brief 释放三级页目录数组
 */
bool_t mmu_del_idirearr(mmudsc_t* mmulocked, idirearr_t* idirearr, msadsc_t* msa);

/**
 * @brief 分配页表数组
 */
msadsc_t* mmu_new_mdirearr(mmudsc_t* mmulocked);

/**
 * @brief 释放页表数组
 */
bool_t mmu_del_mdirearr(mmudsc_t* mmulocked, mdirearr_t* mdirearr, msadsc_t* msa);

/**
 * @brief 取消虚拟地址到物理页的映射
 * @param mmulocked MMU描述符指针
 * @param mdirearr 页表数组
 * @param vadrs 虚拟地址
 * @return 对应的物理地址
 */
adr_t mmu_untransform_msa(mmudsc_t* mmulocked, mdirearr_t* mdirearr, adr_t vadrs);

/**
 * @brief 建立虚拟地址到物理地址的映射
 * @param mmulocked MMU描述符指针
 * @param mdirearr 页表数组
 * @param vadrs 虚拟地址
 * @param padrs 物理地址
 * @param flags 页表标志
 * @return TRUE成功，FALSE失败
 */
bool_t mmu_transform_msa(mmudsc_t* mmulocked, mdirearr_t* mdirearr, adr_t vadrs, adr_t padrs, u64_t flags);

/**
 * @brief 取消页表项映射
 */
bool_t mmu_untransform_mdire(mmudsc_t* mmulocked, idirearr_t* idirearr, msadsc_t* msa, adr_t vadrs);

/**
 * @brief 建立页表项映射
 */
mdirearr_t* mmu_transform_mdire(mmudsc_t* mmulocked, idirearr_t* idirearr, adr_t vadrs, u64_t flags, msadsc_t** outmsa);

/**
 * @brief 取消三级页目录项映射
 */
bool_t mmu_untransform_idire(mmudsc_t* mmulocked, sdirearr_t* sdirearr, msadsc_t* msa, adr_t vadrs);

/**
 * @brief 建立三级页目录项映射
 */
idirearr_t* mmu_transform_idire(mmudsc_t* mmulocked, sdirearr_t* sdirearr, adr_t vadrs, u64_t flags, msadsc_t** outmsa);

/**
 * @brief 取消二级页目录项映射
 */
bool_t mmu_untransform_sdire(mmudsc_t* mmulocked, tdirearr_t* tdirearr, msadsc_t* msa, adr_t vadrs);

/**
 * @brief 建立二级页目录项映射
 */
sdirearr_t* mmu_transform_sdire(mmudsc_t* mmulocked, tdirearr_t* tdirearr, adr_t vadrs, u64_t flags, msadsc_t** outmsa);

/**
 * @brief 地址映射核心函数
 * @param mmu MMU描述符指针
 * @param vadrs 虚拟地址
 * @param padrs 物理地址
 * @param flags 页表标志
 * @return TRUE成功，FALSE失败
 */
bool_t arch_mmu_transform_core(mmudsc_t* mmu, adr_t vadrs, adr_t padrs, u64_t flags);

/**
 * @brief 建立地址映射(外部接口)
 * @param mmu MMU描述符指针
 * @param vadrs 虚拟地址
 * @param padrs 物理地址
 * @param flags 页表标志
 * @return TRUE成功，FALSE失败
 */
bool_t arch_mmu_transform(mmudsc_t* mmu, adr_t vadrs, adr_t padrs, u64_t flags);

/**
 * @brief 查找页表项对应的物理地址
 */
adr_t mmu_find_msaadr(mdirearr_t* mdirearr, adr_t vadrs);

/**
 * @brief 查找页表数组
 */
mdirearr_t* mmu_find_mdirearr(idirearr_t* idirearr, adr_t vadrs);

/**
 * @brief 查找三级页目录数组
 */
idirearr_t* mmu_find_idirearr(sdirearr_t* sdirearr, adr_t vadrs);

/**
 * @brief 查找二级页目录数组
 */
sdirearr_t* mmu_find_sdirearr(tdirearr_t* tdirearr, adr_t vadrs);

/**
 * @brief 取消地址映射核心函数
 * @param mmu MMU描述符指针
 * @param vadrs 虚拟地址
 * @return 对应的物理地址
 */
adr_t arch_mmu_untransform_core(mmudsc_t* mmu, adr_t vadrs);

/**
 * @brief 取消地址映射(外部接口)
 * @param mmu MMU描述符指针
 * @param vadrs 虚拟地址
 * @return 对应的物理地址
 */
adr_t arch_mmu_untransform(mmudsc_t* mmu, adr_t vadrs);

/**
 * @brief 加载MMU页表到CR3
 * @param mmu MMU描述符指针
 */
void arch_mmu_load(mmudsc_t* mmu);

/**
 * @brief 刷新TLB
 */
void arch_mmu_refresh();

/**
 * @brief 初始化MMU
 * @param mmu MMU描述符指针
 * @return TRUE成功，FALSE失败
 */
bool_t arch_mmu_init(mmudsc_t* mmu);

/**
 * @brief 清理页表数组的物理页
 */
bool_t mmu_clean_mdirearrmsas(mmudsc_t* mmulocked);

/**
 * @brief 清理三级页目录数组的物理页
 */
bool_t mmu_clean_idirearrmsas(mmudsc_t* mmulocked);

/**
 * @brief 清理二级页目录数组的物理页
 */
bool_t mmu_clean_sdirearrmsas(mmudsc_t* mmulocked);

/**
 * @brief 清理顶级页目录数组的物理页
 */
bool_t mmu_clean_tdirearrmsas(mmudsc_t* mmulocked);

/**
 * @brief 清理MMU所有页表
 * @param mmu MMU描述符指针
 * @return TRUE成功，FALSE失败
 */
bool_t arch_mmu_clean(mmudsc_t* mmu);

/**
 * @brief 转储MMU信息
 * @param dump MMU描述符指针
 */
void dump_mmu(mmudsc_t* dump);

/*===========================================================================
 * 内联辅助函数
 *===========================================================================*/

/**
 * @brief 获取顶级页目录索引
 * @param vadrs 虚拟地址
 * @return 索引值
 */
KLINE uint_t mmu_tdire_index(adr_t vadrs) {
	return (uint_t)((vadrs >> TDIRE_IV_RSHTBIT) & TDIRE_IV_BITMASK);
}

/**
 * @brief 获取二级页目录索引
 * @param vadrs 虚拟地址
 * @return 索引值
 */
KLINE uint_t mmu_sdire_index(adr_t vadrs) {
	return (uint_t)((vadrs >> SDIRE_IV_RSHTBIT) & SDIRE_IV_BITMASK);
}

/**
 * @brief 获取三级页目录索引
 * @param vadrs 虚拟地址
 * @return 索引值
 */
KLINE uint_t mmu_idire_index(adr_t vadrs) {
	return (uint_t)((vadrs >> IDIRE_IV_RSHTBIT) & IDIRE_IV_BITMASK);
}

/**
 * @brief 获取页表索引
 * @param vadrs 虚拟地址
 * @return 索引值
 */
KLINE uint_t mmu_mdire_index(adr_t vadrs) {
	return (uint_t)((vadrs >> MDIRE_IV_RSHTBIT) & MDIRE_IV_BITMASK);
}

/**
 * @brief 初始化CR3结构
 */
KLINE void cr3s_init(cr3s_t* init) {
    if(NULL == init) {
        return;
    }
    init->c3s_entry = 0;
    return;
}

/**
 * @brief 检查二级页目录数组是否全零
 */
KLINE bool_t sdirearr_is_allzero(sdirearr_t* sdirearr) {
    for(uint_t i = 0; i < SDIRE_MAX; i++) {
        if(0 != sdirearr->sde_arr[i].s_entry) {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * @brief 检查是否有二级页目录
 */
KLINE bool_t sdire_is_have(tdire_t* tdire) {
    if(0 < tdire->t_flags.t_sdir) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief 检查二级页目录是否存在
 */
KLINE bool_t sdire_is_presence(tdire_t* tdire) {
    if(1 == tdire->t_flags.t_p) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief 获取二级页目录的物理地址
 */
KLINE adr_t sdire_ret_padr(tdire_t* tdire) {
    return (adr_t)(tdire->t_flags.t_sdir << SDIRE_PADR_LSHTBIT);
}

/**
 * @brief 获取二级页目录的虚拟地址
 */
KLINE adr_t sdire_ret_vadr(tdire_t* tdire) {
    return phyadr_to_viradr(sdire_ret_padr(tdire));
}

/**
 * @brief 获取二级页目录数组
 */
KLINE sdirearr_t* tdire_ret_sdirearr(tdire_t* tdire) {
    return (sdirearr_t*)(sdire_ret_vadr(tdire));
}

/**
 * @brief 检查三级页目录数组是否全零
 */
KLINE bool_t idirearr_is_allzero(idirearr_t* idirearr) {
    for(uint_t i = 0; i < IDIRE_MAX; i++) {
        if(0 != idirearr->ide_arr[i].i_entry) {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * @brief 检查是否有三级页目录
 */
KLINE bool_t idire_is_have(sdire_t* sdire) {
    if(0 < sdire->s_flags.s_idir) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief 检查三级页目录是否存在
 */
KLINE bool_t idire_is_presence(sdire_t* sdire) {
    if(1 == sdire->s_flags.s_p) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief 获取三级页目录的物理地址
 */
KLINE adr_t idire_ret_padr(sdire_t* sdire) {
    return (adr_t)(sdire->s_flags.s_idir << IDIRE_PADR_LSHTBIT);
}

/**
 * @brief 获取三级页目录的虚拟地址
 */
KLINE adr_t idire_ret_vadr(sdire_t* sdire) {
    return phyadr_to_viradr(idire_ret_padr(sdire));
}

/**
 * @brief 获取三级页目录数组
 */
KLINE idirearr_t* sdire_ret_idirearr(sdire_t* sdire) {
    return (idirearr_t*)(idire_ret_vadr(sdire));
}

/**
 * @brief 检查页表数组是否全零
 */
KLINE bool_t mdirearr_is_allzero(mdirearr_t* mdirearr) {
    for(uint_t i = 0; i < MDIRE_MAX; i++) {
        if(0 != mdirearr->mde_arr[i].m_entry) {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * @brief 检查是否有页表
 */
KLINE bool_t mdire_is_have(idire_t* idire) {
    if(0 < idire->i_flags.i_mdir) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief 检查页表是否存在
 */
KLINE bool_t mdire_is_presence(idire_t* idire) {
    if(1 == idire->i_flags.i_p) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief 获取页表的物理地址
 */
KLINE adr_t mdire_ret_padr(idire_t* idire) {
    return (adr_t)(idire->i_flags.i_mdir << MDIRE_PADR_LSHTBIT);
}

/**
 * @brief 获取页表的虚拟地址
 */
KLINE adr_t mdire_ret_vadr(idire_t* idire) {
    return phyadr_to_viradr(mdire_ret_padr(idire));
}

/**
 * @brief 获取页表数组
 */
KLINE mdirearr_t* idire_ret_mdirearr(idire_t* idire) {
    return (mdirearr_t*)(mdire_ret_vadr(idire));
}

/**
 * @brief 检查是否有物理页映射
 */
KLINE bool_t mmumsa_is_have(mdire_t* mdire) {
    if(0 < mdire->m_flags.m_msa) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief 检查物理页映射是否存在
 */
KLINE bool_t mmumsa_is_presence(mdire_t* mdire) {
    if(1 == mdire->m_flags.m_p) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief 获取映射的物理地址
 */
KLINE adr_t mmumsa_ret_padr(mdire_t* mdire) {
    return (adr_t)(mdire->m_flags.m_msa << MSA_PADR_LSHTBIT);
}

/**
 * @brief 初始化顶级页目录数组
 */
KLINE void tdirearr_init(tdirearr_t* init) {
    if(NULL == init) {
        return;
    }
    arch_memset((void*)init, 0, sizeof(tdirearr_t));
    return;
}

/**
 * @brief 初始化二级页目录数组
 */
KLINE void sdirearr_init(sdirearr_t* init) {
    if(NULL == init) {
        return;
    }
    arch_memset((void*)init, 0, sizeof(sdirearr_t));
    return;
}

/**
 * @brief 初始化三级页目录数组
 */
KLINE void idirearr_init(idirearr_t* init) {
    if(NULL == init) {
        return;
    }
    arch_memset((void*)init, 0, sizeof(idirearr_t));
    return;
}

/**
 * @brief 初始化页表数组
 */
KLINE void mdirearr_init(mdirearr_t* init) {
    if(NULL == init) {
        return;
    }
    arch_memset((void*)init, 0, sizeof(mdirearr_t));
    return;
}

#endif // ARCH_X86_MMU_H
