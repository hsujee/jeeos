/*
 * @Author: Jee Hsu
 * @Description: 架构层内存管理 - 物理内存探测和初始化
 *
 * 负责x86平台的物理内存管理，包括：
 * - 从E820内存映射获取物理内存信息
 * - 初始化物理内存区域结构
 * - 内存区域排序和管理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_ARCH_MM_H
#define ARCH_X86_ARCH_MM_H

#ifdef CFG_X86_PLATFORM

/**
 * @brief 初始化物理内存区域结构
 * @param initp 要初始化的物理内存区域结构指针
 */
PUBLIC void phymmarge_init(phymmarge_t* initp);

/**
 * @brief 获取物理内存区域的地址和大小
 * @param boot_param 引导参数结构指针 (mabp: machine/arch boot parameter)
 * @param ret_phymem_vaddr 返回物理内存区域的虚拟地址
 * @param ret_phymem_size 返回物理内存区域的大小
 */
PUBLIC void ret_phymmarge_adrandsz(abootparam_t* boot_param, phymmarge_t** ret_phymem_vaddr, u64_t* ret_phymem_size);

/**
 * @brief 从E820条目初始化一个物理内存区域
 * @param e820_entry E820内存映射条目指针
 * @param phymem_range 物理内存区域结构指针
 * @return TRUE成功，FALSE失败
 */
PUBLIC bool_t init_one_pmrge(e820map_t* e820_entry, phymmarge_t* phymem_range);

/**
 * @brief 交换两个物理内存区域结构
 * @param src 源结构指针
 * @param dst 目标结构指针
 */
void phymmarge_swap(phymmarge_t* src, phymmarge_t* dst);

/**
 * @brief 对物理内存区域数组进行排序
 * @param phymem_array 物理内存区域数组指针
 * @param count 数组元素个数
 */
void phymmarge_sort(phymmarge_t* phymem_array, u64_t count);

/**
 * @brief 物理内存区域初始化核心函数
 * @param e820_array E820内存映射数组指针
 * @param e820_count E820条目数量
 * @param phymem_array 物理内存区域数组指针
 * @return 初始化的物理内存区域数量
 */
PUBLIC u64_t initpmrge_core(e820map_t* e820_array, u64_t e820_count, phymmarge_t* phymem_array);

/**
 * @brief 初始化物理内存区域
 * 
 * 从BIOS E820内存映射中获取物理内存信息并初始化
 */
PUBLIC void init_phymmarge();

/**
 * @brief 架构层内存初始化入口
 * 
 * 调用init_phymmarge()和init_memmgr()完成内存初始化
 */
void init_archmm();

#endif // CFG_X86_PLATFORM
#endif // ARCH_X86_ARCH_MM_H
