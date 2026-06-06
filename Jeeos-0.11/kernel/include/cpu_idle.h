/*
 * @Author: Jee Hsu
 * @Description: CPU空闲处理 - 空闲线程管理
 *
 * 管理系统空闲时的CPU状态：
 * - 空闲线程创建
 * - 节能状态切换
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _CPUIDLE_H
#define _CPUIDLE_H

/**
 * @brief 初始化CPU空闲子系统
 */
void init_cpuidle();

/**
 * @brief 启动CPU空闲处理
 */
void cpuidle_start();

/**
 * @brief 创建CPU空闲线程
 * @return 空闲线程指针
 */
thread_t* new_cpuidle_thread();

/**
 * @brief 创建新的CPU空闲处理
 */
void new_cpuidle();

/**
 * @brief CPU空闲线程主函数
 * 
 * 在没有其他线程可运行时执行，进入低功耗状态
 */
void cpuidle_main();

#endif // _CPUIDLE_H
