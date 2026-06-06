/*
 * @Author: Jee Hsu
 * @Description: SLAB对象分配器 - 小对象内存分配
 *
 * 实现SLAB风格的对象分配器，用于高效分配小于页面大小的对象：
 * - 对象缓存管理
 * - 对象分配与释放
 * - 缓存扩展机制
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_SLAB_ALLOC_H
#define ARCH_X86_SLAB_ALLOC_H

/**
 * @brief 初始化SLAB缓存链表
 * @param initp 缓存链表指针
 * @param pnr 页面数
 */
void msclst_init(msclst_t* initp, uint_t pnr);

/**
 * @brief 初始化SLAB对象描述符容器
 * @param initp 容器指针
 */
void msomdc_init(msomdc_t* initp);

/**
 * @brief 初始化空闲对象头
 * @param initp 空闲对象头指针
 * @param stus 状态
 * @param stat 起始地址
 */
void freobjh_init(freobjh_t* initp, uint_t stus, void* stat);

/**
 * @brief 初始化SLAB对象结构
 * @param initp SLAB对象指针
 */
void kmsob_init(kmsob_t* initp);

/**
 * @brief 初始化SLAB扩展结构
 * @param initp 扩展结构指针
 * @param vstat 虚拟起始地址
 * @param vend 虚拟结束地址
 * @param kmsp 所属SLAB对象
 */
void kmbext_init(kmbext_t* initp, adr_t vstat, adr_t vend, kmsob_t* kmsp);

/**
 * @brief 初始化SLAB对象链表
 * @param initp 链表指针
 * @param koblsz 对象大小
 */
void koblst_init(koblst_t* initp, size_t koblsz);

/**
 * @brief 初始化SLAB管理头
 * @param initp 管理头指针
 */
void kmsobmgrhed_init(kmsobmgrhed_t* initp);

/**
 * @brief 显示SLAB管理器信息
 */
void disp_kmsobmgr();

/**
 * @brief 显示SLAB对象信息
 * @param kmsp SLAB对象指针
 */
void disp_kmsob(kmsob_t* kmsp);

/**
 * @brief 初始化SLAB分配器
 */
void init_kmsob();

/**
 * @brief 更新SLAB缓存
 * @param kmobmgrp SLAB管理头指针
 * @param koblp 对象链表指针
 * @param kmsp SLAB对象指针
 * @param flgs 操作标志
 */
void kmsob_updata_cache(kmsobmgrhed_t* kmobmgrp, koblst_t* koblp, kmsob_t* kmsp, uint_t flags);

/**
 * @brief 检查SLAB对象能否分配指定大小
 * @param kmsp SLAB对象指针
 * @param msz 请求大小
 * @return 可用的SLAB对象指针，不可用返回NULL
 */
kmsob_t* scan_newkmsob_isok(kmsob_t* kmsp, size_t msz);

/**
 * @brief 检查SLAB对象能否释放指定地址
 * @param kmsp SLAB对象指针
 * @param fadrs 要释放的地址
 * @param fsz 释放大小
 * @return 可用的SLAB对象指针，不可用返回NULL
 */
kmsob_t* scan_delkmsob_isok(kmsob_t *kmsp, void* fadrs, size_t fsz);

/**
 * @brief 检查SLAB对象大小是否匹配
 */
bool_t scan_nmszkmsob_isok(kmsob_t* kmsp, size_t msz);

/**
 * @brief 检查地址是否在SLAB对象范围内
 */
bool_t scan_fadrskmsob_isok(adr_t fostat, adr_t vend, void* fadrs, size_t objsz);

/**
 * @brief 检查释放大小是否匹配
 */
bool_t scan_dfszkmsob_isok(kmsob_t* kmsp, void* fadrs, size_t fsz);

/**
 * @brief 获取SLAB对象中的对象数量
 */
uint_t scan_kmsob_objnr(kmsob_t* kmsp);

/**
 * @brief 从对象链表获取可分配的SLAB对象
 */
kmsob_t* onkoblst_retn_newkmsob(koblst_t* koblp, size_t msz);

/**
 * @brief 从对象链表获取可释放的SLAB对象
 */
kmsob_t* onkoblst_retn_delkmsob(koblst_t *koblp, void* fadrs, size_t fsz);

/**
 * @brief 根据大小获取对象链表
 * @param kmmgrhlokp SLAB管理头指针
 * @param msz 对象大小
 * @return 对象链表指针
 */
koblst_t* onmsz_retn_koblst(kmsobmgrhed_t* kmmgrhlokp, size_t msz);

/**
 * @brief 添加SLAB对象到链表
 */
bool_t kmsob_add_koblst(koblst_t* koblp, kmsob_t* kmsp);

/**
 * @brief 创建并初始化SLAB对象
 */
kmsob_t* _create_init_kmsob(kmsob_t* kmsp, size_t objsz, adr_t cvadrs, adr_t cvadre, msadsc_t* msa, uint_t relpnr);

/**
 * @brief 创建SLAB对象
 */
kmsob_t* _create_kmsob(kmsobmgrhed_t* kmmgrlokp, koblst_t* koblp, size_t objsz);

/**
 * @brief 从SLAB对象分配内存(操作层)
 */
void* kmsob_new_opkmsob(kmsob_t* kmsp, size_t msz);

/**
 * @brief 扩展SLAB对象的页面
 */
bool_t kmsob_extn_pages(kmsob_t* kmsp);

/**
 * @brief 从SLAB对象分配内存
 */
void* kmsob_new_onkmsob(kmsob_t* kmsp, size_t msz);

/**
 * @brief 分配内存核心函数
 */
void* kmsob_new_core(size_t msz);

/**
 * @brief 分配内存(外部接口)
 * @param msz 请求大小
 * @return 分配的内存地址，失败返回NULL
 */
void* kmsob_new(size_t msz);

/**
 * @brief 检查SLAB对象能否释放
 */
uint_t scan_freekmsob_isok(kmsob_t* kmsp);

/**
 * @brief 销毁SLAB对象核心函数
 */
bool_t _destroy_kmsob_core(kmsobmgrhed_t* kmobmgrp, koblst_t* koblp, kmsob_t* kmsp);

/**
 * @brief 销毁SLAB对象
 */
bool_t _destroy_kmsob(kmsobmgrhed_t* kmobmgrp, koblst_t* koblp, kmsob_t* kmsp);

/**
 * @brief 释放SLAB对象中的内存(操作层)
 */
bool_t kmsob_del_opkmsob(kmsob_t* kmsp, void* fadrs, size_t fsz);

/**
 * @brief 释放SLAB对象中的内存
 */
bool_t kmsob_delete_onkmsob(kmsob_t* kmsp, void* fadrs, size_t fsz);

/**
 * @brief 释放内存核心函数
 */
bool_t kmsob_delete_core(void* fadrs, size_t fsz);

/**
 * @brief 释放内存(外部接口)
 * @param fadrs 要释放的地址
 * @param fsz 释放大小
 * @return TRUE成功，FALSE失败
 */
bool_t kmsob_delete(void* fadrs, size_t fsz);

/* 测试和调试函数 */
bool_t chek_kmbext_findmsa(kmsob_t* kmsp, kmbext_t* cpbexp);
bool_t chek_one_kmbext(kmsob_t* kmsp, kmbext_t* cpbexp);
bool_t chek_onekmsob_mbext(kmsob_t* kmsp);
void chek_one_kmsob(kmsob_t* kmsp, size_t objsz);
void chek_all_kmsobstruc();
void kobcks_init(kobcks_t* initp, void* vadr, size_t sz);
void write_kobcks(kmsobmgrhed_t* kmmgrp, void* ptr, size_t sz);
void chek_one_kobcks(kobcks_t* kkp);
void chek_all_kobcks();
void free_one_kobcks(kobcks_t* kkp);
void free_all_kobcks();
void test_kmsob();

#endif // ARCH_X86_SLAB_ALLOC_H
