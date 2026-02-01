/*
 * @Author: Jee Hsu
 * @Description: 进程调度器 - 线程调度管理
 *
 * 实现基于优先级的抢占式调度：
 * - 就绪队列管理
 * - 线程切换
 * - 等待/唤醒机制
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SCHED_H
#define _SCHED_H

/**
 * @brief 初始化线程链表
 * @param initp 线程链表指针
 */
void thrdlst_t_init(thrdlst_t* initp);

/**
 * @brief 初始化调度数据结构
 * @param initp 调度数据指针
 */
void schdata_t_init(schdata_t* initp);

/**
 * @brief 初始化调度类
 * @param initp 调度类指针
 */
void schedclass_t_init(schedclass_t* initp);

/**
 * @brief 初始化调度器
 */
void init_sched();

/**
 * @brief 获取当前线程
 * @return 当前运行的线程指针
 */
thread_t* sched_retn_currthread();

/**
 * @brief 当前线程进入等待状态
 * @param wlst 等待队列
 */
void sched_wait(waitlist_t* wlst);

/**
 * @brief 唤醒等待队列中的线程
 * @param wlst 等待队列
 */
void sched_up(waitlist_t* wlst);

/**
 * @brief 设置调度标志
 */
void sched_set_schedflags();

/**
 * @brief 检查是否需要抢占调度
 */
void sched_chkneed_pmptsched();

/**
 * @brief 选择下一个运行的线程
 * @return 选中的线程指针
 */
thread_t* sched_select_thread();

/**
 * @brief 执行调度
 * 
 * 选择下一个线程并切换上下文
 */
void schedul();

/**
 * @brief 将线程添加到调度类
 * @param thdp 线程指针
 */
void schedclass_add_thread(thread_t* thdp);

/**
 * @brief 切换到新上下文(汇编实现)
 * @param next 下一个线程
 * @param prev 当前线程
 */
#ifdef CFG_X86_PLATFORM
TNCCALL void __to_new_context(thread_t* next, thread_t* prev);
#else
void __to_new_context(thread_t* next, thread_t* prev);
#endif

/**
 * @brief 保存并切换到新上下文
 * @param next 下一个线程
 * @param prev 当前线程
 */
void save_to_new_context(thread_t* next, thread_t* prev);

/**
 * @brief 从首次调度返回
 * @param thrdp 线程指针
 */
void retnfrom_first_sched(thread_t* thrdp);

#endif // _SCHED_H
