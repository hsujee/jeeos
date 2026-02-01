/*
 * @Author: Jee Hsu
 * @Description: 物理页分配器 - 伙伴系统页面分配与释放
 *
 * 实现基于伙伴系统(Buddy System)的物理页分配算法，支持：
 * - 按需分配连续物理页
 * - 页面合并和分割
 * - 内存区域管理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_PAGE_ALLOC_H
#define ARCH_X86_PAGE_ALLOC_H

/**
 * @brief 更新内存管理对象统计信息
 * @param realpnr 实际页面数
 * @param flgs 操作标志
 */
void mm_update_memmgrob(uint_t realpnr, uint_t flags);

/**
 * @brief 更新内存区域统计信息
 * @param malokp 内存区域指针
 * @param pgnr 页面数
 * @param flgs 操作标志
 */
void mm_update_mmarea(mmarea_t* malokp, uint_t pgnr, uint_t flags);

/**
 * @brief 根据物理页描述符获取所属内存区域
 * @param mmobjp 内存管理对象指针
 * @param freemsa 物理页描述符
 * @param freepgs 页面数
 * @return 内存区域指针
 */
mmarea_t* onfrmsa_retn_marea(memmgrob_t* mmobjp, msadsc_t* freemsa, uint_t freepgs);

/**
 * @brief 根据内存类型获取内存区域
 * @param mmobjp 内存管理对象指针
 * @param mrtype 内存区域类型
 * @return 内存区域指针
 */
mmarea_t* onmrtype_retn_marea(memmgrob_t* mmobjp, uint_t mrtype);

/**
 * @brief 获取物理页描述符对应的空闲页面数
 * @param freemsa 物理页描述符
 * @return 空闲页面数
 */
u64_t onfrmsa_retn_fpagenr(msadsc_t* freemsa);

/**
 * @brief 获取内存区域中最大的伙伴链表
 * @param malckp 内存区域指针
 * @return 伙伴链表指针
 */
bafhlst_t* onma_retn_maxbafhlst(mmarea_t* malckp);

/**
 * @brief 分割页面时操作物理页描述符
 * @param msastat 起始描述符
 * @param mnr 描述符数量
 * @return 分割后的描述符
 */
msadsc_t* mm_divpages_opmsadsc(msadsc_t* msastat, uint_t mnr);

/**
 * @brief 合并页面时操作物理页描述符
 * @param bafh 伙伴链表指针
 * @param freemsa 要合并的描述符
 * @param freepgs 页面数
 * @return 成功返回正值，失败返回负值
 */
sint_t mm_merpages_opmsadsc(bafhlst_t* bafh, msadsc_t* freemsa, uint_t freepgs);

/**
 * @brief 根据请求页面数获取合适的伙伴链表
 * @param malckp 内存区域指针
 * @param pages 请求页面数
 * @param retrelbafh 返回释放用伙伴链表
 * @param retdivbafh 返回分割用伙伴链表
 * @return TRUE成功，FALSE失败
 */
bool_t onmpgs_retn_bafhlst(mmarea_t* malckp, uint_t pages, bafhlst_t** retrelbafh, bafhlst_t** retdivbafh);

/**
 * @brief 根据释放页面数获取合适的伙伴链表
 * @param malckp 内存区域指针
 * @param freepgs 释放页面数
 * @param retrelbf 返回释放用伙伴链表
 * @param retmerbf 返回合并用伙伴链表
 * @return TRUE成功，FALSE失败
 */
bool_t onfpgs_retn_bafhlst(mmarea_t* malckp, uint_t freepgs, bafhlst_t** retrelbf, bafhlst_t** retmerbf);

/**
 * @brief 从伙伴链表分割页面
 * @param bafhp 伙伴链表指针
 * @return 分割得到的物理页描述符
 */
msadsc_t* mm_divipages_onbafhlst(bafhlst_t* bafhp);

/**
 * @brief 从伙伴链表获取物理页描述符范围
 * @param bafhp 伙伴链表指针
 * @param retmstat 返回起始描述符
 * @param retmend 返回结束描述符
 * @return TRUE成功，FALSE失败
 */
bool_t mm_retnmsaob_onbafhlst(bafhlst_t* bafhp, msadsc_t** retmstat, msadsc_t** retmend);

/**
 * @brief 检查内存区域能否分配指定页面数
 * @param malckp 内存区域指针
 * @param pages 请求页面数
 * @return TRUE可以分配，FALSE不能分配
 */
bool_t scan_mapgsalloc_ok(mmarea_t* malckp, uint_t pages);

/**
 * @brief 从内存区域分配最大可用页面
 * @param malckp 内存区域指针
 * @param retrelpnr 返回实际分配页面数
 * @return 分配得到的物理页描述符
 */
msadsc_t* mm_maxdivpages_onmarea(mmarea_t* malckp, uint_t* retrelpnr);

/**
 * @brief 检查分割的描述符长度
 */
uint_t chek_divlenmsa(msadsc_t* msastat, msadsc_t* msaend, uint_t mnr);

/**
 * @brief 将物理页描述符添加到伙伴链表
 */
bool_t mrdmb_add_msa_bafh(bafhlst_t* bafhp, msadsc_t* msastat, msadsc_t* msaend);

/**
 * @brief 分配页面并分割
 */
msadsc_t* mm_reldpgsdivmsa_bafhl(mmarea_t *malckp, uint_t pages, uint_t *retrelpnr, bafhlst_t* relbfl, bafhlst_t* divbfl);

/**
 * @brief 从内存区域分配并分割页面
 */
msadsc_t* mm_reldivpages_onmarea(mmarea_t* malckp, uint_t pages, uint_t* retrelpnr);

/**
 * @brief 处理内存区域页面分配
 */
msadsc_t* mm_prcdivpages_onmarea(mmarea_t *malckp, uint_t pages, uint_t *retrelpnr);

/**
 * @brief 页面分配核心函数
 * @param mareap 内存区域指针
 * @param pages 请求页面数
 * @param retrealpnr 返回实际分配页面数
 * @param flgs 分配标志
 * @return 分配得到的物理页描述符
 */
msadsc_t* mm_divpages_core(mmarea_t* mareap, uint_t pages, uint_t* retrealpnr, uint_t flags);

/**
 * @brief 页面分配框架函数
 */
msadsc_t* mm_divpages_fmwk(memmgrob_t* mmobjp, uint_t pages, uint_t* retrelpnr, uint_t mrtype, uint_t flags);

/**
 * @brief 分配物理页面(外部接口)
 * @param mmobjp 内存管理对象指针
 * @param pages 请求页面数
 * @param retrealpnr 返回实际分配页面数
 * @param mrtype 内存区域类型
 * @param flgs 分配标志
 * @return 分配得到的物理页描述符
 */
msadsc_t* mm_division_pages(memmgrob_t* mmobjp, uint_t pages, uint_t*retrealpnr, uint_t mrtype, uint_t flags);

/**
 * @brief 获取进程内存区域
 */
mmarea_t* retn_procmarea(memmgrob_t *mmobjp);

/**
 * @brief 从进程内存区域分配页面核心函数
 */
msadsc_t* divpages_procmarea_core(memmgrob_t *mmobjp, uint_t pages, uint_t *retrealpnr);

/**
 * @brief 从进程内存区域分配页面
 */
msadsc_t* mm_divpages_procmarea(memmgrob_t *mmobjp, uint_t pages, uint_t *retrealpnr);

/**
 * @brief 检查待释放的物理页是否有效
 */
bool_t scan_freemsa_isok(msadsc_t* freemsa, uint_t freepgs);

/**
 * @brief 检查单个内存块是否可合并
 */
sint_t mm_cmsa1blk_isok(bafhlst_t* bafh, msadsc_t* _1ms, msadsc_t* _1me);

/**
 * @brief 检查两个内存块是否可合并
 */
sint_t mm_cmsa2blk_isok(bafhlst_t* bafh, msadsc_t* _1ms, msadsc_t* _1me, msadsc_t* _2ms, msadsc_t* _2me);

/**
 * @brief 检查并清除合并锁标志
 */
bool_t chek_cl2molkflag(bafhlst_t* bafh, msadsc_t* _1ms, msadsc_t* _1me, msadsc_t* _2ms, msadsc_t* _2me);

/**
 * @brief 清除两个内存块的合并标志
 */
bool_t mm_clear_2msaolflag(bafhlst_t* bafh, msadsc_t* _1ms, msadsc_t* _1me, msadsc_t* _2ms, msadsc_t* _2me);

/**
 * @brief 查找可合并的两个内存块
 */
sint_t mm_find_cmsa2blk(bafhlst_t* fbafh, msadsc_t** rfnms, msadsc_t** rfnme);

/**
 * @brief 添加物理页描述符到伙伴链表
 */
bool_t mpobf_add_msadsc(bafhlst_t* bafhp, msadsc_t* freemstat, msadsc_t* freemend);

/**
 * @brief 在伙伴链表上合并页面
 */
bool_t mm_merpages_onbafhlst(msadsc_t* freemsa, uint_t freepgs, bafhlst_t* relbf, bafhlst_t* merbf);

/**
 * @brief 在内存区域上合并页面
 */
bool_t mm_merpages_onmarea(mmarea_t* malckp, msadsc_t* freemsa, uint_t freepgs);

/**
 * @brief 合并页面核心函数
 */
bool_t mm_merpages_core(mmarea_t* marea, msadsc_t* freemsa, uint_t freepgs);

/**
 * @brief 合并页面框架函数
 */
bool_t mm_merpages_fmwk(memmgrob_t* mmobjp, msadsc_t* freemsa, uint_t freepgs);

/**
 * @brief 合并(释放)物理页面(外部接口)
 * @param mmobjp 内存管理对象指针
 * @param freemsa 要释放的物理页描述符
 * @param freepgs 页面数
 * @return TRUE成功，FALSE失败
 */
bool_t mm_merge_pages(memmgrob_t* mmobjp, msadsc_t* freemsa, uint_t freepgs);

/* 测试相关函数 */
void mchkstuc_init(mchkstuc_t* initp);
void write_one_mchkstuc(msadsc_t* msa, uint_t pnr);
bool_t chek_one_mchks(mchkstuc_t* mchs);
void cmp_mchkstuc(mchkstuc_t* smchs, mchkstuc_t* dmchs);
void free_one_mchkstuc(mchkstuc_t* mchs);
void free_all_mchkstuc();
void chek_all_one_mchkstuc(mchkstuc_t* mchs);
void chek_all_mchkstuc();
uint_t retn_test_sec();
void test_proc_marea(mmarea_t* mr);
void test_maxone_marea(mmarea_t* mr);
void test_onedivmer_all(mmarea_t* ma);
void test_maxdiv_all();
void test_divsion_pages();

#endif // ARCH_X86_PAGE_ALLOC_H
