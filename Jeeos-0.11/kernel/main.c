/*
 * @Author: Jee Hsu
 * @Description: 内核主入口 - Jeeos内核启动的入口点
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "main.h"
#include "global_types.h"
#include "global_heads.h"

/**
 * @brief 内核初始化入口函数
 * 
 * 由架构层调用，完成内核核心子系统的初始化：
 * 1. 内存管理器初始化
 * 2. 时间子系统初始化（在调度器之前，因为调度器可能需要时间信息）
 * 3. 设备子系统初始化
 * 4. 调度器初始化（在驱动之前，因为定时器中断处理函数依赖调度器）
 * 5. 驱动程序加载（定时器驱动注册中断处理函数）
 * 6. CPU空闲进程初始化
 * 
 * @note 此函数不返回，进入die()后开始调度
 */
void init_kernel() {
    printk("[KERNEL] Initializing memory manager...\n");
    init_mm();
    
    printk("[KERNEL] Initializing time subsystem...\n");
    init_ktime();
    
    printk("[KERNEL] Initializing scheduler...\n");
    init_sched();
    
    printk("[KERNEL] Initializing device subsystem...\n");
    init_device();
    
    printk("[KERNEL] Loading drivers...\n");
    init_driver();
    
    printk("[KERNEL] All subsystems ready.\n");
    printk("[KERNEL] Creating idle process...\n");
    
    /* 启动 CPU idle 进程（不返回） */
    init_cpuidle();
    
    die(0);
    return;
}
