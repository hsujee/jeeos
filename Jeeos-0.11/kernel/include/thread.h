/*
 * @Author: Jee Hsu
 * @Description: 线程管理 - 线程创建与操作
 *
 * 提供线程生命周期管理：
 * - 线程描述符分配
 * - 用户/内核线程创建
 * - 线程栈初始化
 * - 线程句柄管理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _THREAD_H
#define _THREAD_H

/**
 * @brief 初始化微栈结构
 * @param initp 微栈指针
 */
void micrstk_t_init(micrstk_t* initp);

/**
 * @brief 初始化上下文结构
 * @param initp 上下文指针
 */
void context_t_init(context_t* initp);

/**
 * @brief 获取线程ID
 * @param tdp 线程指针
 * @return 线程ID
 */
uint_t thread_get_id(thread_t* tdp);

/**
 * @brief 初始化线程结构
 * @param initp 线程指针
 */
void thread_t_init(thread_t* initp);

/**
 * @brief 分配新的线程描述符
 * @return 线程描述符指针，失败返回NULL
 */
thread_t* new_thread_dsc();

/**
 * @brief 增加线程时间片计数
 * @param thdp 线程指针
 */
void thread_inc_tick(thread_t* thdp);

/**
 * @brief 获取空句柄
 * @param thdp 线程指针
 * @return 空句柄值
 */
hand_t thread_get_nullhand(thread_t* thdp);

/**
 * @brief 向线程添加对象节点
 * @param thdp 线程指针
 * @param ondp 对象节点指针
 * @return 对象句柄
 */
hand_t thread_add_objnode(thread_t* thdp, objnode_t* ondp);

/**
 * @brief 从线程删除对象节点
 * @param thdp 线程指针
 * @param hand 对象句柄
 * @return 删除的句柄
 */
hand_t thread_del_objnode(thread_t *thdp, hand_t hand);

/**
 * @brief 获取线程的对象节点
 * @param thdp 线程指针
 * @param hand 对象句柄
 * @return 对象节点指针
 */
objnode_t* thread_get_objnode(thread_t *thdp, hand_t hand);

/**
 * @brief 初始化用户栈
 * @param thdp 线程指针
 * @param runadr 运行地址
 * @param cpuflags CPU标志
 */
void thread_userstack_init(thread_t *thdp, void *runadr, uint_t cpuflags);

/**
 * @brief 初始化内核栈
 * @param thdp 线程指针
 * @param runadr 运行地址
 * @param cpuflags CPU标志
 */
void thread_kernstack_init(thread_t *thdp, void *runadr, uint_t cpuflags);

/**
 * @brief 创建用户线程核心函数
 * @param filerun 运行入口地址
 * @param flg 线程标志
 * @param prilg 特权级
 * @param prity 优先级
 * @param usrstksz 用户栈大小
 * @param krlstksz 内核栈大小
 * @return 线程指针，失败返回NULL
 */
thread_t* new_user_thread_core(void* filerun, uint_t flg, uint_t prilg, uint_t prity, size_t usrstksz, size_t krlstksz);

/**
 * @brief 创建内核线程核心函数
 * @param filerun 运行入口地址
 * @param flg 线程标志
 * @param prilg 特权级
 * @param prity 优先级
 * @param usrstksz 用户栈大小
 * @param krlstksz 内核栈大小
 * @return 线程指针，失败返回NULL
 */
thread_t* new_kern_thread_core(void* filerun, uint_t flg, uint_t prilg, uint_t prity, size_t usrstksz, size_t krlstksz);

/**
 * @brief 创建新线程(外部接口)
 * @param filerun 运行入口地址
 * @param flg 线程标志
 * @param prilg 特权级
 * @param prity 优先级
 * @param usrstksz 用户栈大小
 * @param krlstksz 内核栈大小
 * @return 线程指针，失败返回NULL
 */
thread_t* new_thread(void* filerun, uint_t flg, uint_t prilg, uint_t prity, size_t usrstksz, size_t krlstksz);

#endif // _THREAD_H
