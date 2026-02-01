/*
 * @Author: Jee Hsu
 * @Description: 系统服务层依赖头文件
 *
 * 系统调用模块的精确依赖聚合，替代全局聚合方案
 * 只包含系统调用实现所需的必要头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_DEPS_H
#define _SYS_DEPS_H

/* ====== 系统配置 ====== */
#include "config.h"

/* ====== 基础类型 ====== */
#include "btypes.h"         /* 基础类型定义 */

/* ====== 架构层类型(完整聚合) ====== */
#include "arch_types.h"     /* 架构层类型聚合 */

/* ====== 内核层类型(完整聚合) ====== */
#include "kernel_types.h"   /* 内核层类型聚合 */

/* ====== 系统服务层类型(kernel_heads依赖) ====== */
#include "systypes.h"       /* 系统调用类型(syscall_t等) */

/* ====== 架构层接口(完整聚合) ====== */
#include "arch_heads.h"     /* 架构层接口聚合 */

/* ====== 内核层接口(完整聚合) ====== */
#include "kernel_heads.h"   /* 内核层接口聚合 */

/* ====== 系统服务层接口 ====== */
#include "sysheads.h"       /* 系统调用接口 */

#endif // _SYS_DEPS_H

