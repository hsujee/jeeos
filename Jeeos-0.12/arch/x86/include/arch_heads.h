/*
 * @Author: Jee Hsu
 * @Description: 架构层头文件聚合
 *
 * 包含所有架构层相关的头文件，方便统一引用
 *
 * ============================================================================
 * arch_ 前缀命名规范:
 * ============================================================================
 *
 * 使用 arch_ 前缀的函数:
 *   1. 硬件操作函数 - 直接访问CPU/内存硬件
 *      - arch_spinlock_xxx()   : 自旋锁(使用lock前缀指令)
 *      - arch_sti/cli_cpuflag(): 中断标志操作
 *      - arch_wbinvd/invd()    : 缓存操作
 *      - arch_memset/memcpy()  : 优化的内存操作
 *
 *   2. MMU管理函数 - 页表和地址转换
 *      - arch_mmu_transform()  : 虚拟地址映射
 *      - arch_mmu_load()       : 加载CR3寄存器
 *      - arch_mmu_init/clean() : MMU描述符管理
 *
 *   3. 架构特定接口 - 内核调用的架构抽象
 *      - arch_add_ihandle()    : 注册中断处理程序
 *      - arch_logo()           : 显示启动Logo
 *      - arch_sysdie()         : 系统崩溃处理
 *
 * 不使用 arch_ 前缀的函数:
 *   1. 初始化函数 - init_xxx() 格式
 *      - init_phymmarge()      : 物理内存初始化
 *      - init_memmgr()         : 内存管理器初始化
 *      - init_archmm()         : 架构层内存初始化入口
 *
 *   2. 内部实现函数 - 不暴露给内核的辅助函数
 *      - phymmarge_sort()      : 内存区域排序
 *      - mmudsc_init()         : 结构初始化
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_ARCH_HEADS_H
#define ARCH_X86_ARCH_HEADS_H

/* 基础模块 */
#include "link.h"
#include "platform.h"
#include "cpu.h"
#include "cpu_ops.h"
#include "mmu.h"
#include "arch_trap.h"
#include "arch_mm.h"
#include "mach.h"

/* x86平台特定模块 */
#ifdef CFG_X86_PLATFORM
#include "io.h"
#include "idt.h"
#include "i8259.h"
#include "video.h"
#include "mmheads.h"
#endif

/* 全局和初始化模块 */
#include "arch_global.h"
#include "arch_init.h"
#include "printk.h"

#endif // ARCH_X86_ARCH_HEADS_H

