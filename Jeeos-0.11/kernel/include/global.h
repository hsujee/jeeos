/*
 * @Author: Jee Hsu
 * @Description: 内核层全局变量声明
 *
 * 声明内核使用的全局变量：
 * - 虚拟内存空间管理
 * - 内存池
 * - 调度类
 * - 系统时间
 * - 系统调用表
 * - 设备表
 * - 驱动入口表
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _GLOBAL_H
#define _GLOBAL_H

#ifdef	KRLGOBAL_HEAD
#undef	KEXTERN
#define KEXTERN
#endif

#if((defined CFG_X86_PLATFORM) || (defined CFG_S3C2440A_PLATFORM))   

/** 内核虚拟地址空间列表 */
KRL_DEFGLOB_VARIABLE(kvirmemadrs_t, krlvirmemadrs);

/** 初始内存地址描述符 */
KRL_DEFGLOB_VARIABLE(mmadrsdsc_t, initmmadrsdsc);

/** 内核内存池 */
KRL_DEFGLOB_VARIABLE(kmempool_t, oskmempool);

/** 系统调度类 */
KRL_DEFGLOB_VARIABLE(schedclass_t, osschedcls);

/** 系统内核时间 */
KRL_DEFGLOB_VARIABLE(ktime_t, osktime);

/** 系统调用表 */
KRL_DEFGLOB_VARIABLE(syscall_t, osservicetab)[INR_MAX];

/** 设备管理表 */
KRL_DEFGLOB_VARIABLE(devtable_t, osdevtable);

/** 驱动入口表 */
KRL_DEFGLOB_VARIABLE(drventyexit_t, osdrvetytabl)[];

#endif

#endif // _GLOBAL_H
