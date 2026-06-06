/*
 * @Author: Jee Hsu
 * @Description: 信号量实现 - 同步原语
 *
 * 提供计数信号量实现，用于进程/线程同步
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SEM_H
#define _SEM_H

/**
 * @brief 初始化信号量结构
 * @param initp 信号量指针
 */
void sem_t_init(sem_t* initp);

/**
 * @brief 设置信号量
 * @param setsem 信号量指针
 * @param flg 标志
 * @param conut 初始计数值
 */
void sem_set(sem_t* setsem, uint_t flg, sint_t conut);

/**
 * @brief 信号量P操作(等待/减少)
 * 
 * 如果计数为0，当前线程进入等待状态
 * 
 * @param sem 信号量指针
 */
void sem_down(sem_t* sem);

/**
 * @brief 信号量V操作(释放/增加)
 * 
 * 增加计数，如果有等待线程则唤醒
 * 
 * @param sem 信号量指针
 */
void sem_up(sem_t* sem);

#endif // _SEM_H
