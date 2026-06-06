/*
 * @Author: Jee Hsu
 * @Description: I/O缓存模块 - 块设备缓存
 *
 * 提供块设备I/O缓存管理，减少磁盘访问
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _IOCACHE_H
#define _IOCACHE_H

/**
 * @brief 初始化I/O缓存块描述符
 * @param initp 描述符指针
 */
void iocheblkdsc_t_init(iocheblkdsc_t* initp);

/**
 * @brief 初始化I/O缓存块
 * @param initp 缓存块指针
 */
void iocheblk_t_init(iocheblk_t* initp);

/**
 * @brief 初始化I/O缓存块子系统
 */
void init_iocacheblk();

/**
 * @brief 分配新的I/O缓存块
 * @return 缓存块指针，失败返回NULL
 */
iocheblk_t* new_iocheblk();

/**
 * @brief 删除I/O缓存块
 * @param icblkp 缓存块指针
 * @return TRUE成功，FALSE失败
 */
bool_t del_iocheblk(iocheblk_t* icblkp);

/**
 * @brief 查找I/O缓存块
 * @param icblkdsclokp 缓存块描述符
 * @param icblknr 块号
 * @param icblksz 块大小
 * @return 找到的缓存块指针，未找到返回NULL
 */
iocheblk_t* iocheblk_find_iocheblk(iocheblkdsc_t* icblkdsclokp, uint_t icblknr, uint_t icblksz);

/**
 * @brief 分配I/O缓存块核心函数
 */
iocheblk_t* iocheblk_alloc_core(iocheblkdsc_t* icblkdsc, uint_t icblknr, uint_t icblksz);

/**
 * @brief 分配I/O缓存块
 * @param icblkdsc 缓存块描述符
 * @param icblknr 块号
 * @param icblksz 块大小
 * @return 缓存块指针
 */
iocheblk_t* iocheblk_alloc(iocheblkdsc_t* icblkdsc, uint_t icblknr, uint_t icblksz);

/**
 * @brief 释放I/O缓存块核心函数
 */
bool_t iocheblk_free_core(iocheblkdsc_t* icblkdsc, uint_t icblknr, uint_t icblksz);

/**
 * @brief 释放I/O缓存块
 * @param icblkdsc 缓存块描述符
 * @param icblknr 块号
 * @param icblksz 块大小
 * @return TRUE成功，FALSE失败
 */
bool_t iocheblk_free(iocheblkdsc_t* icblkdsc, uint_t icblknr, uint_t icblksz);

/**
 * @brief I/O缓存块测试函数
 */
void iocheblk_test();

#endif // _IOCACHE_H
