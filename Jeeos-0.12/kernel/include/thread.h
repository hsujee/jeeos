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
 * @brief 设置线程名称
 * @param tdp 线程指针
 * @param name 线程名称
 */
void thread_set_name(thread_t *tdp, const char *name);

/**
 * @brief 创建新线程(外部接口)
 * @param filerun 运行入口地址
 * @param flg 线程标志
 * @param prilg 特权级
 * @param prity 优先级
 * @param usrstksz 用户栈大小
 * @param krlstksz 内核栈大小
 * @param name 线程名称
 * @return 线程指针，失败返回NULL
 */
thread_t* new_thread(void* filerun, uint_t flg, uint_t prilg, uint_t prity, 
                     size_t usrstksz, size_t krlstksz, const char *name);

/**
 * @brief 获取当前运行的线程
 */
#define get_current_thread() sched_retn_currthread()

/*===========================================================================
 * PID 管理
 *===========================================================================*/

/**
 * @brief 分配新的 PID
 * @return 新分配的 PID
 */
uint_t alloc_pid(void);

/**
 * @brief 获取当前最大 PID
 * @return 最大 PID 值
 */
uint_t get_max_pid(void);

/*===========================================================================
 * Fork 机制
 *===========================================================================*/

/**
 * @brief 添加子进程到父进程列表
 * @param parent 父进程
 * @param child 子进程
 */
void thread_add_child(thread_t *parent, thread_t *child);

/**
 * @brief 从父进程列表移除子进程
 * @param child 子进程
 */
void thread_remove_child(thread_t *child);

/**
 * @brief Fork 进程
 * @param parent 父进程
 * @param child_entry 子进程入口函数
 * @param child_name 子进程名称
 * @return 子进程指针，失败返回 NULL
 */
thread_t *fork_thread(thread_t *parent, void *child_entry, const char *child_name);

#endif // _THREAD_H
