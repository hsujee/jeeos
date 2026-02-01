/*
 * @Author: Jee Hsu
 * @Description: 内存池管理 - 分层内存分配
 *
 * 提供分层的内存分配机制：
 * - 页面级分配
 * - 对象级分配
 * - 内存池管理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _MMPAGES_H
#define _MMPAGES_H

/**
 * @brief 初始化内存页管理
 */
void init_mmpages();

/**
 * @brief 初始化内存池结构
 * @param initp 内存池指针
 */
void kmempool_t_init(kmempool_t* initp);

/**
 * @brief 调用HAL层分配页面
 * @param msize 请求大小
 * @return 分配的内存地址
 */
adr_t kmempool_page_new_callhalmm(size_t msize);

/**
 * @brief 调用HAL层释放页面
 * @param fradr 要释放的地址
 * @param frze 释放大小
 * @return TRUE成功，FALSE失败
 */
bool_t kmempool_page_delete_callhalmm(adr_t fradr, size_t frze);

/*===========================================================================
 * 内存池头管理
 *===========================================================================*/

/**
 * @brief 初始化内存池头
 * @param initp 内存池头指针
 */
void mplhead_t_init(mplhead_t* initp);

/**
 * @brief 从内存池头删除对象
 */
bool_t objs_delete_on_mplhead(mplhead_t* mplhdp, adr_t fradr);

/**
 * @brief 从内存池头分配对象
 */
adr_t objs_new_on_mplhead(mplhead_t* mplhdp);

/**
 * @brief 从内存池头删除页面
 */
bool_t page_delete_on_mplhead(mplhead_t* mplhdp, adr_t fradr);

/**
 * @brief 从内存池头分配页面
 */
adr_t page_new_on_mplhead(mplhead_t* mplhdp);

/*===========================================================================
 * 内存池初始化
 *===========================================================================*/

/**
 * @brief 初始化对象内存池
 */
mplhead_t* objs_mpool_init(kmempool_t* kmplockp, mplhead_t* initp, size_t msize, adr_t start, adr_t end);

/**
 * @brief 初始化页面内存池
 */
mplhead_t* page_mpool_init(kmempool_t* kmplockp, mplhead_t* initp, size_t msize, adr_t start, adr_t end);

/**
 * @brief 删除对象内存池
 */
bool_t del_objs_mpool(kmempool_t* kmplockp, mplhead_t* mphdp);

/**
 * @brief 删除页面内存池
 */
bool_t del_page_mpool(kmempool_t* kmplockp, mplhead_t* mphdp);

/**
 * @brief 创建新的对象内存池
 */
mplhead_t* new_objs_mpool(kmempool_t* kmplockp, size_t msize);

/**
 * @brief 创建新的页面内存池
 */
mplhead_t* new_page_mpool(kmempool_t* kmplockp, size_t msize);

/*===========================================================================
 * 内存池查找
 *===========================================================================*/

mplhead_t* objsdel_mplhead_isok(mplhead_t* mhp, adr_t fradr, size_t msize);
mplhead_t* pagedel_mplhead_isok(mplhead_t* mhp, adr_t fradr, size_t msize);
mplhead_t* objsnew_mplhead_isok(mplhead_t* mhp, size_t msize);
mplhead_t* pagenew_mplhead_isok(mplhead_t* mhp, size_t msize);
mplhead_t* kmemplob_retn_mplhead(kmempool_t* kmplockp, size_t msize);
mplhead_t* kmempldelpg_retn_mplhead(kmempool_t* kmplockp, adr_t fradr, size_t msize);
mplhead_t* kmemplpg_retn_mplhead(kmempool_t* kmplockp, size_t msize);

/*===========================================================================
 * 内存分配接口
 *===========================================================================*/

/**
 * @brief 页面分配核心函数
 */
adr_t kmempool_pages_core_new(size_t msize);
bool_t kmempool_pages_core_delete(adr_t fradr, size_t frsz);

/**
 * @brief 对象分配核心函数
 */
adr_t kmempool_objsz_core_new(size_t msize);
bool_t kmempool_objsz_core_delete(adr_t fradr, size_t frsz);

/**
 * @brief 对象分配接口
 */
adr_t kmempool_objsz_new(size_t msize);
bool_t kmempool_objsz_delete(adr_t fradr, size_t frsz);

/**
 * @brief 页面分配接口
 */
adr_t kmempool_pages_new(size_t msize);
bool_t kmempool_pages_delete(adr_t fradr, size_t frsz);

/**
 * @brief 按大小分配接口
 */
adr_t kmempool_onsize_new(size_t msize);
bool_t kmempool_onsize_delete(adr_t fradr, size_t frsz);

/**
 * @brief 通用内存分配接口
 * @param msize 请求大小
 * @return 分配的内存地址，失败返回NULL
 */
adr_t kmempool_new(size_t msize);

/**
 * @brief 通用内存释放接口
 * @param fradr 要释放的地址
 * @param frsz 释放大小
 * @return TRUE成功，FALSE失败
 */
bool_t kmempool_delete(adr_t fradr, size_t frsz);

#endif // _MMPAGES_H
