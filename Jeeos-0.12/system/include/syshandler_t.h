/*
 * @Author: Jee Hsu
 * @Description: 系统服务 - syshandler_t
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_HANDLER_T_H
#define _SYS_HANDLER_T_H

#include "typedef.h"  /* u64_t, uint_t, sysstus_t 等基础类型 */

#define INR_MM_ALLOC 0x1UL
#define INR_MM_FREE 0x2UL
#define INR_TD_EXEL 0x3UL
#define INR_TD_EXIT 0x4UL
#define INR_TD_HAND 0x5UL
#define INR_TD_RSTATS 0x6UL
#define INR_TD_SSTATS 0x7UL
#define INR_FS_OPEN 0x8UL
#define INR_FS_CLOSE 0x9UL
#define INR_FS_READ 0xaUL
#define INR_FS_WRITE 0xbUL
#define INR_FS_IOCTRL 0xcUL
#define INR_FS_LSEEK 0xdUL
#define INR_TIME 0xeUL

#define INR_MAX 0xfUL

#define SYSSTUSERR (-1)
#define SYSSTUSOK (0)
#define FILE_TY_MASK 0xf
#define FILE_TY_FILE 0
#define FILE_TY_DEV 1
#define RWO_FLG_MASK 0xff
#define RWO_FLG_BITS 0x4

/* 用户态接口使用的标志（与 libtypes.h 一致，避免重复定义） */
#ifndef RW_FLG
#define RW_FLG (0x3 << RWO_FLG_BITS)
#endif
#ifndef RO_FLG
#define RO_FLG (0x1 << RWO_FLG_BITS)
#endif
#ifndef WO_FLG
#define WO_FLG (0x2 << RWO_FLG_BITS)
#endif
#ifndef NF_FLG
#define NF_FLG (0x4 << RWO_FLG_BITS)
#endif

/* 内部权限检查使用的值（on_acsflags 存储移位后的值） */
#define ACS_RW  0x3   /* 读写权限 */
#define ACS_RO  0x1   /* 只读权限 */
#define ACS_WO  0x2   /* 只写权限 */
#define ACS_NF  0x4   /* 无标志 */

//服务函数类型
typedef struct s_STKPARAME {
    u64_t gs;
	u64_t fs;
	u64_t es;
	u64_t ds;
	u64_t r15;
	u64_t r14;
	u64_t r13;
	u64_t r12;
	u64_t r11;
	u64_t r10;
	u64_t r9;
	u64_t r8;
	
	u64_t parmv5;//rdi;
	u64_t parmv4;//rsi;
	u64_t rbp;
	u64_t parmv3;//rdx;
	u64_t parmv2;//rcx;
	u64_t parmv1;//rbx;

    u64_t rvsrip;
    u64_t rvscs;
    u64_t rvsrflags;
    u64_t rvsrsp;
    u64_t rvsss;
} stkparame_t;

typedef sysstus_t (*syscall_t)(uint_t inr,stkparame_t* stkparm);
#endif // _SYS_HANDLER_T_H
