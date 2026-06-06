/*
 * @Author: Jee Hsu
 * @Description: 内存区域管理 - 伙伴系统内存区域初始化
 *
 * 管理物理内存区域(mmarea_t)，包括：
 * - 内存区域的初始化
 * - 伙伴链表的建立
 * - 连续物理页的组织和管理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_MEM_ZONE_H
#define ARCH_X86_MEM_ZONE_H

/**
 * @brief 初始化区域链表结构
 * @param initp 区域链表结构指针
 */
void arclst_init(arclst_t* initp);

/**
 * @brief 默认的内存区域初始化函数
 * @param mmarea 内存区域指针
 * @param valp 参数值指针
 * @param val 参数值
 * @return 内存状态码
 */
mmstus_t mafo_deft_init(struct s_MMAREA* mmarea, void* valp, uint_t val);

/**
 * @brief 默认的内存区域退出函数
 * @param mmarea 内存区域指针
 * @return 内存状态码
 */
mmstus_t mafo_deft_exit(struct s_MMAREA* mmarea);

/**
 * @brief 默认的内存区域分配释放函数
 * @param mmarea 内存区域指针
 * @param mafrspack 分配结果包
 * @param valp 参数值指针
 * @param val 参数值
 * @return 内存状态码
 */
mmstus_t mafo_deft_afry(struct s_MMAREA* mmarea, mmafrets_t* mafrspack, void* valp, uint_t val);

/**
 * @brief 初始化内存区域函数对象
 * @param initp 函数对象指针
 */
void mafuncobjs_init(mafuncobjs_t* initp);

/**
 * @brief 初始化伙伴链表结构
 * @param initp 伙伴链表指针
 * @param stus 状态
 * @param oder 阶数
 * @param oderpnr 该阶页面数
 */
void bafhlst_init(bafhlst_t* initp, u32_t stus, uint_t oder, uint_t oderpnr);

/**
 * @brief 初始化内存分割结构
 * @param initp 内存分割结构指针
 */
void mmdivide_init(mmdivide_t* initp);

/**
 * @brief 初始化内存区域结构
 * @param initp 内存区域指针
 */
void mmarea_init(mmarea_t* initp);

/**
 * @brief 内存区域初始化核心函数
 * @param mabp 引导参数结构指针
 * @return TRUE成功，FALSE失败
 */
bool_t init_mmarea_core(abootparam_t* mabp);

/**
 * @brief 初始化所有内存区域
 */
void init_mmarea();

/**
 * @brief 在内存区域中查找物理页描述符段
 * @param mareap 内存区域指针
 * @param fmstat 起始描述符
 * @param fmsanr 描述符数量
 * @param retmsastatp 返回起始描述符
 * @param retmsaendp 返回结束描述符
 * @param retfmnr 返回实际数量
 * @return TRUE成功，FALSE失败
 */
bool_t find_inmarea_msadscsegmant(mmarea_t* mareap, msadsc_t* fmstat, uint_t fmsanr, msadsc_t** retmsastatp, msadsc_t** retmsaendp, uint_t* retfmnr);

/**
 * @brief 检查两个物理页描述符是否连续
 * @param prevmsa 前一个描述符
 * @param nextmsa 后一个描述符
 * @param cmpmdfp 比较标志
 * @return 连续返回非零值
 */
uint_t continumsadsc_is_ok(msadsc_t* prevmsa, msadsc_t* nextmsa, msadflgs_t* cmpmdfp);

/**
 * @brief 扫描连续的物理页描述符长度
 */
bool_t scan_len_msadsc(msadsc_t* mstat, msadflgs_t* cmpmdfp, uint_t mnr, uint_t* retmnr);

/**
 * @brief 检查连续物理页描述符
 */
uint_t check_continumsadsc(mmarea_t* mareap, msadsc_t* stat, msadsc_t* end, uint_t fmnr);

/**
 * @brief 合并时扫描连续物理页描述符
 */
bool_t merlove_scan_continumsadsc(mmarea_t* mareap, msadsc_t* fmstat, uint_t* fntmsanr, uint_t fmsanr, msadsc_t** retmsastatp, msadsc_t** retmsaendp, uint_t* retfmnr);

/**
 * @brief 清除连续物理页描述符的标志
 */
uint_t merlove_clrflge_onmsadsc(msadsc_t* mstat, uint_t msanr);

/**
 * @brief 设置内存区域上物理页的标志
 */
uint_t merlove_setflags_onmmarea(mmarea_t* mareap, msadsc_t* mstat, uint_t msanr);

/**
 * @brief 测试设置标志
 */
uint_t test_setflags(mmarea_t* mareap, msadsc_t* mstat, uint_t msanr);

/**
 * @brief 查找可容纳指定页面数的伙伴链表
 */
bafhlst_t* find_continumsa_inbafhlst(mmarea_t* mareap, uint_t fmnr);

/**
 * @brief 添加连续物理页到进程内存区域伙伴链表
 */
bool_t continumsadsc_add_procmareabafh(mmarea_t* mareap, bafhlst_t* bafhp, msadsc_t* fstat, msadsc_t* fend, uint_t fmnr);

/**
 * @brief 添加连续物理页到伙伴链表
 */
bool_t continumsadsc_add_bafhlst(mmarea_t* mareap, bafhlst_t* bafhp, msadsc_t* fstat, msadsc_t* fend, uint_t fmnr);

/**
 * @brief 将连续物理页添加到内存区域伙伴链表核心函数
 */
bool_t continumsadsc_mareabafh_core(mmarea_t* mareap, msadsc_t** rfstat, msadsc_t** rfend, uint_t* rfmnr);

/**
 * @brief 合并连续物理页到内存区域伙伴链表
 */
bool_t merlove_continumsadsc_mareabafh(mmarea_t* mareap, msadsc_t* mstat, msadsc_t* mend, uint_t mnr);

/**
 * @brief 在内存区域上合并内存
 */
bool_t merlove_mem_onmmarea(mmarea_t* mareap, msadsc_t* mstat, uint_t msanr);

/**
 * @brief 合并内存核心函数
 */
bool_t merlove_mem_core(abootparam_t* mabp);

/**
 * @brief 初始化并合并空闲内存
 * 
 * 将所有空闲物理页组织到伙伴系统中
 */
void init_merlove_mem();

/**
 * @brief 显示伙伴链表信息
 * @param bafhp 伙伴链表指针
 */
void disp_bafhlst(bafhlst_t* bafhp);

/**
 * @brief 显示内存区域信息
 * @param mabp 引导参数结构指针
 */
void disp_mmarea(abootparam_t* mabp);

#endif // ARCH_X86_MEM_ZONE_H
