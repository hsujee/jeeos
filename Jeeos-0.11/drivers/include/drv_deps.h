/*
 * @Author: Jee Hsu
 * @Description: 驱动层依赖头文件
 *
 * 驱动模块的精确依赖聚合，替代全局聚合方案
 * 只包含驱动开发所需的必要头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _DRV_DEPS_H
#define _DRV_DEPS_H

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

/* ====== 驱动层类型 ====== */
#include "drv_types.h"      /* 驱动类型定义 */

/* ====== 架构层接口(完整聚合) ====== */
#include "arch_heads.h"     /* 架构层接口聚合 */

/* ====== 内核层接口(完整聚合) ====== */
#include "kernel_heads.h"   /* 内核层接口聚合 */

/* ====== 驱动层接口 ====== */
#include "drv_heads.h"      /* 驱动接口 */

#endif // _DRV_DEPS_H

