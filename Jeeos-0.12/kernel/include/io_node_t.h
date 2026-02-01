/*
 * @Author: Jee Hsu
 * @Description: I/O对象节点类型定义
 *
 * 定义内核对象(设备/文件)访问的统一接口结构：
 * - objnode_t: 对象节点(类似文件描述符)
 *
 * 对象节点是进程访问设备和文件的句柄
 * 通过统一的接口支持设备I/O和文件I/O
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _OBJNODE_T_H
#define _OBJNODE_T_H

/**
 * @brief I/O对象节点结构
 * 
 * 代表一个打开的设备或文件，是进程进行I/O操作的句柄
 * 类似于UNIX的文件描述符(file descriptor)
 */
typedef struct s_OBJNODE {
    spinlock_t on_lock;     /**< 保护对象节点的自旋锁 */
    list_h_t on_list;       /**< 链表节点 */
    sem_t on_complesem;     /**< I/O完成信号量(用于同步) */
    uint_t on_flags;        /**< 对象标志 */
    uint_t on_status;         /**< 对象状态 */
    sint_t on_opercode;     /**< 当前操作码 */
    uint_t on_objtype;      /**< 对象类型: OBJN_TY_xxx */
    void* on_objadr;        /**< 对象地址(设备或文件) */
    uint_t on_acsflags;      /**< 访问标志(读/写/执行) */
    uint_t on_acsstus;      /**< 访问状态 */
    uint_t on_currops;      /**< 当前读写位置(文件偏移) */
    uint_t on_len;          /**< 读写数据长度 */
    uint_t on_ioctrd;       /**< I/O控制码 */
    buf_t on_buf;           /**< I/O缓冲区 */
    uint_t on_bufcurops;    /**< 缓冲区当前位置 */
    size_t on_bufsz;        /**< 缓冲区大小 */
    uint_t on_count;        /**< 引用计数 */
    void* on_safedsc;       /**< 安全描述符(权限控制) */
    void* on_fname;         /**< 文件路径名指针 */
    void* on_finode;        /**< 文件节点指针 */
    void* on_extp;          /**< 扩展数据 */
} objnode_t;

/** @name 对象类型定义 */
/** @{ */
#define OBJN_TY_NUL 0       /**< 空类型 */
#define OBJN_TY_DEV 1       /**< 设备类型 */
#define OBJN_TY_FIL 2       /**< 文件类型 */
/** @} */

#endif // OBJNODE_T_H
