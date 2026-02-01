/*
 * @Author: Jee Hsu
 * @Description: 架构层类型定义聚合
 *
 * 包含所有架构层相关的类型定义头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_ARCH_TYPES_H
#define ARCH_X86_ARCH_TYPES_H

/* 基础类型定义 */
#include "platform_t.h"
#include "cpu_t.h"
#include "cpu_ops_t.h"
#include "mmu_t.h"
#include "arch_trap_t.h"
#include "arch_mm_t.h"
#include "mach_t.h"

/* x86平台特定类型 */
#ifdef CFG_X86_PLATFORM
#include "idt_t.h"
#include "pages64_t.h"
#include "video_t.h"
#include "mm_types.h"
#endif

/* 全局类型定义 */
#include "arch_global_t.h"
#include "arch_init_t.h"
#include "printk_t.h"

#endif // ARCH_X86_ARCH_TYPES_H

