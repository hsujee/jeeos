/*
 * @Author: Jee Hsu
 * @Description: 内核主函数入口
 *
 * 定义内核初始化入口点
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef MAIN_H
#define MAIN_H

/**
 * @brief 内核初始化入口函数
 * 
 * 由架构层调用，完成内核核心子系统的初始化：
 * 1. 内存管理器
 * 2. 调度器
 * 3. 时间子系统
 * 4. CPU空闲处理
 */
void init_kernel();

#endif // MAIN_H
