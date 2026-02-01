/**
 * @file mm_types.h
 * @author Jee Hsu
 * @brief 内存管理类型定义聚合
 *
 * 包含所有内存管理相关的类型定义头文件:
 * - page_desc_t.h: 页描述符
 * - mem_zone_t.h: 内存区域
 * - page_alloc_t.h: 页分配器
 * - slab_alloc_t.h: Slab分配器
 * - mm_init_t.h: 内存初始化
 *
 * @copyright Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_MM_TYPES_H
#define ARCH_X86_MM_TYPES_H
#include "page_desc_t.h"
#include "mem_zone_t.h"
#include "page_alloc_t.h"
#include "slab_alloc_t.h"
#include "mm_init_t.h"
#endif // ARCH_X86_MM_TYPES_H
