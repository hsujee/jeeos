/*
 * @Author: Jee Hsu
 * @Description: 全局头文件总聚合
 *
 * 顶层头文件，包含配置、架构层、内核层、系统调用层和驱动层的所有头文件
 *
 * 使用场景(混合模式)：
 * - kernel/ 和 arch/ 核心模块使用此文件
 * - drivers/ 使用 drv_deps.h (精确包含)
 * - system/ 使用 sys_deps.h (精确包含)
 * - libs/ 使用 libtypes.h + libheads.h (精确包含)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef GLOBAL_HEADS_H
#define GLOBAL_HEADS_H

#include "config.h"       /* 系统配置 */
#include "arch_heads.h"   /* 架构层头文件 */
#include "kernel_heads.h" /* 内核层头文件 */
#include "sysheads.h"     /* 系统调用层头文件 */
#include "drv_heads.h"    /* 驱动层头文件 */
#include "libheads.h"     /* 用户库头文件 */

#endif // GLOBAL_HEADS_H

