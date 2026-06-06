/*
 * @Author: Jee Hsu
 * @Description: 架构初始化模块 - x86平台初始化入口
 *
 * 负责x86架构的底层初始化，包括平台、内存和中断子系统。
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_ARCH_INIT_H
#define ARCH_X86_ARCH_INIT_H

/**
 * @brief 架构层初始化入口
 * 
 * 按顺序初始化x86平台的各个子系统：
 * 1. init_archplalt() - 平台初始化（图形驱动、硬件检测）
 * 2. init_archmm()    - 内存初始化（MMU、页表、内存管理）
 * 3. init_archint()   - 中断初始化（GDT/IDT、中断处理）
 */
void init_arch();

#endif // ARCH_X86_ARCH_INIT_H
