/*
 * @Author: Jee Hsu
 * @Description: 内核全局变量定义
 *
 * 定义内核核心子系统的全局变量：
 * - krlvirmemadrs: 内核虚拟地址空间管理
 * - initmmadrsdsc: 初始内存地址描述符
 * - oskmempool: 内核内存池
 * - osschedcls: 调度器类
 * - osktime: 系统时间
 *
 * 使用KRL_DEFGLOB_VARIABLE宏将变量放入.data段
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#define KRLGOBAL_HEAD
#include "global_types.h"
#include "global_heads.h"

KRL_DEFGLOB_VARIABLE(kvirmemadrs_t, krlvirmemadrs);
KRL_DEFGLOB_VARIABLE(mmadrsdsc_t, initmmadrsdsc);
KRL_DEFGLOB_VARIABLE(kmempool_t, oskmempool);
KRL_DEFGLOB_VARIABLE(schedclass_t, osschedcls);
KRL_DEFGLOB_VARIABLE(ktime_t, osktime);
