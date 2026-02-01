/*
 * @Author: Jee Hsu
 * @Description: 内存管理器初始化
 *
 * 初始化内存管理子系统，包括：
 * - 页表初始化
 * - 显存映射
 * - 内存管理对象初始化
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_MM_INIT_H
#define ARCH_X86_MM_INIT_H

/**
 * @brief 初始化内存管理器
 * 
 * 完成页表拷贝、显存映射、内存管理对象初始化
 */
LKINIT void init_memmgr();

/**
 * @brief 显示内存管理对象信息
 */
void disp_memmgrob();

/**
 * @brief 初始化内存管理对象
 */
void init_memmgrob();

/**
 * @brief 初始化内存管理对象结构
 * @param initp 内存管理对象指针
 */
void memmgrob_init(memmgrob_t* initp);

/**
 * @brief 拷贝页表数据
 * @param mabp 引导参数结构指针
 * @return TRUE成功，FALSE失败
 */
bool_t copy_pages_data(abootparam_t* mabp);

/**
 * @brief 拷贝显存数据
 * @param mabp 引导参数结构指针
 * @param dgp 图形设备指针
 * @return TRUE成功，FALSE失败
 */
bool_t copy_fvm_data(abootparam_t* mabp, dftgraph_t* dgp);

/**
 * @brief 初始化页表和显存拷贝
 */
void init_copy_pagesfvm();

#endif // ARCH_X86_MM_INIT_H
