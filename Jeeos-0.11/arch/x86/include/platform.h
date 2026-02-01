/*
 * @Author: Jee Hsu
 * @Description: 平台初始化 - 硬件平台相关功能
 *
 * 提供x86平台的基础功能：
 * - 虚拟地址和物理地址转换
 * - 引导参数处理
 * - 内核镜像加载
 * - 地址范围检查
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_PLATFORM_H
#define ARCH_X86_PLATFORM_H

/**
 * @brief 虚拟地址转物理地址
 * @param kviradr 内核虚拟地址
 * @return 对应的物理地址，失败返回KRNL_ADDR_ERROR
 */
adr_t viradr_to_phyadr(adr_t kviradr);

/**
 * @brief 物理地址转虚拟地址
 * @param kphyadr 物理地址
 * @return 对应的内核虚拟地址，失败返回KRNL_ADDR_ERROR
 */
adr_t phyadr_to_viradr(adr_t kphyadr);

/**
 * @brief 初始化引导参数结构
 * @param initp 引导参数结构指针
 */
void init_abootparam_t(abootparam_t *initp);

/**
 * @brief 初始化引导参数
 * 
 * 从二级引导器复制机器信息到全局变量
 */
void init_abootparam();

/**
 * @brief 平台初始化入口
 * 
 * 初始化引导参数、图形驱动、重定位内核镜像
 */
void init_archplalt();

/**
 * @brief 获取文件信息
 * @param fname 文件名
 * @param mabp 引导参数结构指针
 * @return 文件描述符指针，未找到返回NULL
 */
fhdsc_t *get_fileinfo(char_t *fname, abootparam_t *mabp);

/**
 * @brief 获取文件的虚拟地址和大小
 * @param fname 文件名
 * @param mabp 引导参数结构指针
 * @param retadr 返回文件地址
 * @param retsz 返回文件大小
 */
void get_file_rvadrandsz(char_t *fname, abootparam_t *mabp, u64_t *retadr, u64_t *retsz);

/**
 * @brief 移动内核镜像到最大物理地址
 * @param mabp 引导参数结构指针
 */
void move_img2maxpadr(abootparam_t *mabp);

/**
 * @brief 检查地址区域是否有效
 * @param sadr 起始地址
 * @param slen 长度
 * @param kadr 内核地址
 * @param klen 内核长度
 * @return 0有效，非0无效
 */
int adrzone_is_ok(u64_t sadr, u64_t slen, u64_t kadr, u64_t klen);

/**
 * @brief 检查初始化地址是否有效
 * @param mabp 引导参数结构指针
 * @param chkadr 要检查的地址
 * @param cksz 大小
 * @return 0有效，非0无效
 */
int initchkadr_is_ok(abootparam_t *mabp, u64_t chkadr, u64_t cksz);

#endif // ARCH_X86_PLATFORM_H
