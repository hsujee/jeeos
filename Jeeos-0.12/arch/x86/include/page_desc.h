/*
 * @Author: Jee Hsu
 * @Description: 物理页描述符管理 - 物理内存页面跟踪
 *
 * 管理物理内存页面的描述符(msadsc_t)，包括：
 * - 物理页描述符的初始化
 * - 物理页地址和状态查询
 * - 内核占用内存区域标记
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_PAGE_DESC_H
#define ARCH_X86_PAGE_DESC_H

/**
 * @brief 初始化物理页描述符
 * @param initp 要初始化的描述符指针
 */
void msadsc_init(msadsc_t* initp);

/**
 * @brief 显示单个物理页描述符信息
 * @param mp 物理页描述符指针
 */
void disp_one_msadsc(msadsc_t* mp);

/**
 * @brief 获取物理页描述符数组的地址和数量
 * @param mabp 引导参数结构指针
 * @param retmasvp 返回描述符数组的虚拟地址
 * @param retmasnr 返回描述符数量
 * @return TRUE成功，FALSE失败
 */
bool_t ret_msadsc_vadrandsz(abootparam_t* mabp, msadsc_t** retmasvp, u64_t* retmasnr);

/**
 * @brief 写入单个物理页描述符
 * @param msap 物理页描述符指针
 * @param phyadr 物理地址
 */
void write_one_msadsc(msadsc_t* msap, u64_t phyadr);

/**
 * @brief 物理页描述符初始化核心函数
 * @param mabp 引导参数结构指针
 * @param msavstart 描述符数组起始地址
 * @param msanr 描述符数量
 * @return 初始化的描述符数量
 */
LKINIT u64_t init_msadsc_core(abootparam_t* mabp, msadsc_t* msavstart, u64_t msanr);

/**
 * @brief 初始化所有物理页描述符
 */
LKINIT void init_msadsc();

/**
 * @brief 显示所有物理页描述符
 */
void disp_phymsadsc();

/**
 * @brief 搜索指定状态范围内的物理页描述符
 * @param msastart 描述符数组起始地址
 * @param msanr 描述符数量
 * @param ocpystat 占用状态起始值
 * @param ocpyend 占用状态结束值
 * @return 找到的描述符数量
 */
u64_t search_segment_occupymsadsc(msadsc_t* msastart, u64_t msanr, u64_t ocpystat, u64_t ocpyend);

/**
 * @brief 搜索内核占用的物理页描述符
 * @param mabp 引导参数结构指针
 * @return TRUE成功，FALSE失败
 */
bool_t search_krloccupymsadsc_core(abootparam_t* mabp);

/**
 * @brief 初始化并搜索内核占用的内存
 * @param mabp 引导参数结构指针
 */
void init_search_krloccupymm(abootparam_t* mabp);

/**
 * @brief 获取物理页描述符对应的物理地址
 * @param msa 物理页描述符指针
 * @return 物理地址，失败返回NULL
 */
KLINE adr_t msadsc_ret_addr(msadsc_t* msa) {
    if(NULL == msa) {
        return NULL;
    }
    return (msa->md_phyadrs.paf_padrs << PAGPHYADR_SZLSHBIT);
}

/**
 * @brief 获取物理页描述符对应的虚拟地址
 * @param msa 物理页描述符指针
 * @return 虚拟地址，失败返回NULL
 */
KLINE adr_t msadsc_ret_vaddr(msadsc_t* msa) {
    if(NULL == msa) {
        return NULL;
    }	
    return phyadr_to_viradr(msadsc_ret_addr(msa));
}

#endif // ARCH_X86_PAGE_DESC_H
