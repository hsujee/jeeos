/*
 * @Author: Jee Hsu
 * @Description: 对象池分配器类型定义
 *
 * 定义内核对象池(Slab)分配器的数据结构：
 * - objmplctrl_t: 对象池控制器(全局唯一)
 * - objalst_t: 对象分配链表
 * - objmgrhead_t: 对象管理头部
 *
 * 对象池用于高效分配固定大小的内核对象
 * 类似于Linux的SLAB分配器
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _OBJSMPOL_T_H
#define _OBJSMPOL_T_H

#define OBJALST_MAX 15          /**< 对象分配链表最大数量 */
#define OBJALCSZ_MAX 2048       /**< 最大分配对象大小 */

/**
 * @brief 对象池配置数据
 */
typedef struct s_OLCFGDAT {
    size_t ocd_objsz;           /**< 对象大小 */
    size_t ocd_nmpsz;           /**< 名称池大小 */
} olcfgdat_t;

/**
 * @brief 对象管理头部结构
 * 
 * 管理一块内存中的固定大小对象
 */
typedef struct s_OBJMGRHEAD {
    list_h_t omh_list;          /**< 链表节点 */
    struct s_OBJALST* omh_paralst; /**< 所属分配链表 */
    size_t omh_objsz;           /**< 对象原始大小 */
    size_t omh_aligobjsz;       /**< 对齐后对象大小 */
    size_t omh_npinobjsz;       /**< 页内对象大小 */
    uint_t omh_objnr;           /**< 对象总数 */
    uint_t omh_alfrindx;        /**< 分配/释放索引 */
    adr_t omh_start;            /**< 管理区起始地址 */
    adr_t omh_end;              /**< 管理区结束地址 */
    adr_t omh_freeobjp;         /**< 空闲对象指针 */
} objmgrhead_t;

/**
 * @brief 对象分配链表结构
 * 
 * 管理特定大小的对象池
 */
typedef struct s_OBJALST {
    list_h_t oal_list;          /**< 部分空闲的管理头链表 */
    list_h_t oal_emplist;       /**< 完全空闲的管理头链表 */
    olcfgdat_t* oal_cfgmsz;     /**< 配置数据 */
    uint_t oal_emplstnr;        /**< 空闲链表数量 */
    uint_t oal_lstnr;           /**< 链表数量 */
    objmgrhead_t* oal_omhcahe;  /**< 管理头缓存 */
} objalst_t;

/**
 * @brief 对象池控制器结构(全局唯一)
 * 
 * 管理所有大小类别的对象池
 */
typedef struct s_OBJMPLCTRL {
    spinlock_t omc_lock;        /**< 保护控制器的自旋锁 */
    objalst_t* omc_alstcahe;    /**< 分配链表缓存 */
    objalst_t omc_alclst[OBJALST_MAX]; /**< 各大小类别的分配链表 */
} objmplctrl_t;

#endif // OBJSMPOL_T_H
