/*
 * @Author: Jee Hsu
 * @Description: 内核启动入口 - C语言部分
 *
 * 由entry.asm在完成64位长模式初始化后调用
 *
 * 启动流程：
 * GRUB -> bootsect.asm -> setup.asm -> entry.asm -> start_kernel()
 *
 * start_kernel()执行流程：
 * 1. init_arch() - 初始化架构层(平台/内存/中断)
 * 2. init_kernel() - 初始化内核子系统
 * 3. 进入调度循环(不返回)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

/**
 * @brief 内核启动入口
 * 
 * 由引导程序跳转至此，开始内核初始化流程
 */
void start_kernel() {
    /* 初始化架构层（包括视频驱动） */
    init_arch();
    
    /* 浅绿色显示启动信息 */
    pixl_t oldpx = set_deffontpx(BGRA(0x90, 0xee, 0x90));
    printk("[Jeeos-0.11] Kernel Starting...\n");
    set_deffontpx(oldpx);
    
    /* 初始化内核子系统 */
    init_kernel();

    return;
}
