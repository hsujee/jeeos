/*
 * @Author: Jee Hsu
 * @Description: 虚拟地址管理 - 虚拟内存区域管理
 *
 * 管理进程的虚拟地址空间：
 * - 虚拟内存区域(VMA)分配和释放
 * - 页面映射和取消映射
 * - 缺页处理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _VIRADDRESS_H
#define _VIRADDRESS_H

/*===========================================================================
 * 测试结构函数
 *===========================================================================*/

void teststc_t_init(teststc_t *initp);
teststc_t *new_teststc();
void del_teststc(teststc_t *delstc);
void add_new_teststc(adr_t vadr, size_t vsiz);

/*===========================================================================
 * 虚拟地址空间结构初始化
 *===========================================================================*/

/**
 * @brief 初始化虚拟地址空间锁节点
 * @param initp 锁节点指针
 */
void vaslknode_t_init(vaslknode_t *initp);

/**
 * @brief 初始化页表页结构
 * @param initp 页表页指针
 */
void pgtabpage_t_init(pgtabpage_t *initp);

/**
 * @brief 初始化内核虚拟内存区域描述符
 * @param initp 描述符指针
 */
void kmvarsdsc_t_init(kmvarsdsc_t *initp);

/**
 * @brief 初始化虚拟内存地址结构
 * @param initp 结构指针
 */
void virmemadrs_t_init(virmemadrs_t *initp);

/**
 * @brief 初始化内核虚拟内存地址结构
 * @param initp 结构指针
 */
void kvirmemadrs_t_init(kvirmemadrs_t *initp);

/*===========================================================================
 * 虚拟内存区域管理
 *===========================================================================*/

/**
 * @brief 分配内核虚拟内存区域描述符
 * @return 描述符指针，失败返回NULL
 */
kmvarsdsc_t *new_kmvarsdsc();

/**
 * @brief 删除内核虚拟内存区域描述符
 * @param delkmvd 要删除的描述符
 * @return TRUE成功，FALSE失败
 */
bool_t del_kmvarsdsc(kmvarsdsc_t *delkmvd);

/**
 * @brief 分配虚拟内存地址结构
 * @return 结构指针，失败返回NULL
 */
virmemadrs_t *new_virmemadrs();

/**
 * @brief 删除虚拟内存地址结构
 * @param vmdsc 要删除的结构
 * @return TRUE成功，FALSE失败
 */
bool_t del_virmemadrs(virmemadrs_t *vmdsc);

/**
 * @brief 初始化内核虚拟内存地址空间
 */
void init_kvirmemadrs();

/*===========================================================================
 * 虚拟地址查找和分配
 *===========================================================================*/

/**
 * @brief 查找虚拟内存区域描述符
 */
kmvarsdsc_t *vma_find_kmvarsdsc_is_ok(virmemadrs_t *vmalocked, kmvarsdsc_t *curr, adr_t start, size_t vassize);
kmvarsdsc_t *vma_find_kmvarsdsc(virmemadrs_t *vmalocked, adr_t start, size_t vassize);

/**
 * @brief 分配新的虚拟地址空间
 * @param mm 内存地址描述符
 * @param start 起始地址(0表示自动分配)
 * @param vassize 大小
 * @param vaslimits 限制标志
 * @param vastype 类型
 * @return 分配的虚拟地址，失败返回0
 */
adr_t vma_new_vadrs_core(mmadrsdsc_t *mm, adr_t start, size_t vassize, u64_t vaslimits, u32_t vastype);
adr_t vma_new_vadrs(mmadrsdsc_t *mm, adr_t start, size_t vassize, u64_t vaslimits, u32_t vastype);

/*===========================================================================
 * 虚拟地址释放
 *===========================================================================*/

kmvarsdsc_t *vma_del_find_kmvarsdsc(virmemadrs_t *vmalocked, adr_t start, size_t vassize);
void vma_del_set_endcurrkmvd(virmemadrs_t *vmalocked, kmvarsdsc_t *del);
bool_t vma_del_unmapping_phyadrs(mmadrsdsc_t *mm, kmvarsdsc_t *kmvd, adr_t start, adr_t end);
bool_t vma_del_unmapping(mmadrsdsc_t *mm, kmvarsdsc_t *kmvd, adr_t start, size_t vassize);

/**
 * @brief 释放虚拟地址空间
 * @param mm 内存地址描述符
 * @param start 起始地址
 * @param vassize 大小
 * @return TRUE成功，FALSE失败
 */
bool_t vma_del_vadrs_core(mmadrsdsc_t *mm, adr_t start, size_t vassize);
bool_t vma_del_vadrs(mmadrsdsc_t *mm, adr_t start, size_t vassize);

/*===========================================================================
 * 测试函数
 *===========================================================================*/

int imrand();
int kvma_rand(int s, int e);
void chk_one_kmva(kmvarsdsc_t *chkkmva);
void check_kmva();
teststc_t *find_tstc_on_rnr(uint_t randnr);
void rand_write_tstc();
void rand_del_tstc();
void check_one_teststc(teststc_t *chktc);
void check_teststc();
void del_all_kvma();
void __test_kmva();
void test_kmva();
void test_vma();

/*===========================================================================
 * 页表管理
 *===========================================================================*/

/**
 * @brief 获取当前CPU的PML4基地址
 */
adr_t kmap_retcurentcpu_pml4badrs();

/**
 * @brief 获取当前CPU的CR3值
 */
cr3s_t knl_retn_currentcpu_cr3s();

/**
 * @brief 刷新当前CPU的MMU页表
 */
void kmap_fulshcurrcpu_mmutable();

/*===========================================================================
 * 缺页处理
 *===========================================================================*/

kmvarsdsc_t *vma_map_find_kmvarsdsc(virmemadrs_t *vmalocked, adr_t vadrs);
kvmemcbox_t *vma_map_retn_kvmemcbox(kmvarsdsc_t *kmvd);
bool_t vma_del_usermsa(mmadrsdsc_t *mm, kvmemcbox_t *kmbox, msadsc_t *msa, adr_t phyadr);
msadsc_t *vma_new_usermsa(mmadrsdsc_t *mm, kvmemcbox_t *kmbox);
adr_t vma_map_msa_fault(mmadrsdsc_t *mm, kvmemcbox_t *kmbox, adr_t vadrs, u64_t flags);
adr_t vma_map_phyadrs(mmadrsdsc_t *mm, kmvarsdsc_t *kmvd, adr_t vadrs, u64_t flags);

/**
 * @brief 缺页异常处理核心函数
 * @param mm 内存地址描述符
 * @param vadrs 发生缺页的虚拟地址
 * @return 0成功，负值失败
 */
sint_t vma_map_fairvadrs_core(mmadrsdsc_t *mm, adr_t vadrs);
sint_t vma_map_fairvadrs(mmadrsdsc_t *mm, adr_t vadrs);

/**
 * @brief 用户空间访问失败处理
 * @param fairvadrs 失败的虚拟地址
 * @return 0成功，负值失败
 */
sint_t userspace_accessfailed(adr_t fairvadrs);

void test_krl_pages_fault();

/*===========================================================================
 * 虚拟内存缓存对象管理
 *===========================================================================*/

void kvmcobjmgr_t_init(kvmcobjmgr_t *initp);
void kvmemcobj_t_init(kvmemcobj_t *initp);
kvmemcobj_t *new_kvmemcobj();
bool_t del_kvmemcobj(kvmemcobj_t *delkvmcop);
void kvmemcbox_t_init(kvmemcbox_t *init);
void kvmemcboxmgr_t_init(kvmemcboxmgr_t *init);
kvmemcbox_t *new_kvmemcbox();
bool_t del_kvmemcbox(kvmemcbox_t *del);
void count_kvmemcbox(kvmemcbox_t *kmbox);
void decount_kvmemcbox(kvmemcbox_t *kmbox);
kvmemcbox_t *get_kvmemcbox();
bool_t put_kvmemcbox(kvmemcbox_t *kmbox);
void dump_kvmemcboxmgr(kvmemcboxmgr_t *dump);

#endif // _VIRADDRESS_H
