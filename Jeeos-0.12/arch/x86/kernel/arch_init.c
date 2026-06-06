/*
 * @Author: Jee Hsu
 * @Description: x86架构初始化模块
 *
 * 负责x86-64平台的底层硬件初始化
 * 是HAL(硬件抽象层)的入口点
 *
 * 初始化顺序(不可更改)：
 * 1. 平台初始化 - 视频驱动必须最先，否则无法使用printk
 * 2. 内存初始化 - 设置页表、物理页管理、SLAB分配器
 * 3. 中断初始化 - 设置GDT/IDT、8259 PIC、中断处理函数
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

/**
 * @brief 架构层初始化入口
 * 
 * 按顺序初始化x86平台的各个子系统：
 * 1. 平台初始化 - 设置图形驱动、硬件检测
 * 2. 内存初始化 - 设置MMU、页表、内存管理
 * 3. 中断初始化 - 设置GDT/IDT、中断处理
 */
// 早期串口调试
extern void early_serial_puts(const char* s);

void init_arch() {
    /* 初始化平台（包括视频驱动） */
    init_archplalt();
    printk("[ARCH] Platform initialized.\n");
    
    /* 初始化内存管理 */
    printk("[ARCH] Initializing memory...\n");
    init_archmm();
    
    /* 初始化中断系统 */
    printk("[ARCH] Initializing interrupts...\n");
    init_archint();
    
    printk("[ARCH] Architecture layer ready.\n");
}
