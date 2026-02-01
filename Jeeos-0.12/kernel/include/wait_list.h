/*
 * @Author: Jee Hsu
 * @Description: 等待队列 - 线程同步机制
 *
 * 提供线程等待和唤醒机制
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _WAITLIST_H
#define _WAITLIST_H

/**
 * @brief 初始化等待队列
 * @param initp 等待队列指针
 */
void waitlist_t_init(waitlist_t* initp);

/**
 * @brief 当前线程进入等待队列
 * @param wlst 等待队列指针
 */
void waitlist_wait(waitlist_t* wlst);

/**
 * @brief 唤醒等待队列中的一个线程
 * @param wlst 等待队列指针
 */
void waitlist_up(waitlist_t* wlst);

/**
 * @brief 唤醒等待队列中的所有线程
 * @param wlst 等待队列指针
 */
void waitlist_allup(waitlist_t* wlst);

/**
 * @brief 将线程添加到等待队列
 * @param wlst 等待队列指针
 * @param tdp 线程指针
 */
void waitlist_add_thread(waitlist_t* wlst, thread_t* tdp);

/**
 * @brief 从等待队列移除线程
 * @param wlst 等待队列指针
 * @return 移除的线程指针
 */
thread_t* waitlist_del_thread(waitlist_t *wlst);

#endif // _WAITLIST_H
