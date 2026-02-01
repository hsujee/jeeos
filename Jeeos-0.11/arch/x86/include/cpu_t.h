/*
 * @Author: Jee Hsu
 * @Description: x86-64 CPU寄存器和中断栈帧类型定义
 *
 * 定义中断/异常处理时的栈帧结构：
 * - intstkregs_t: 普通中断栈帧(无错误码)
 * - faultstkregs_t: 异常栈帧(带错误码)
 *
 * 注意：栈向下增长，结构体从低地址到高地址定义
 * 实际压栈顺序与结构体字段顺序相反
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_CPU_T_H
#define ARCH_X86_CPU_T_H

#ifdef CFG_X86_PLATFORM

/**
 * @brief 普通中断栈帧结构(无错误码)
 * 
 * 中断发生时CPU自动保存的寄存器+软件保存的寄存器
 * 用于中断处理程序保存/恢复上下文
 */
typedef struct s_INTSTKREGS {
    /* 软件保存的段寄存器 */
    uint_t r_gs;            /**< GS段寄存器 */
    uint_t r_fs;            /**< FS段寄存器 */
    uint_t r_es;            /**< ES段寄存器 */
    uint_t r_ds;            /**< DS段寄存器 */
    /* 软件保存的通用寄存器 */
    uint_t r_r15;           /**< R15寄存器 */
    uint_t r_r14;           /**< R14寄存器 */
    uint_t r_r13;           /**< R13寄存器 */
    uint_t r_r12;           /**< R12寄存器 */
    uint_t r_r11;           /**< R11寄存器 */
    uint_t r_r10;           /**< R10寄存器 */
    uint_t r_r9;            /**< R9寄存器 */
    uint_t r_r8;            /**< R8寄存器 */
    uint_t r_rdi;           /**< RDI寄存器(参数1) */
    uint_t r_rsi;           /**< RSI寄存器(参数2) */
    uint_t r_rbp;           /**< RBP寄存器(栈帧指针) */
    uint_t r_rdx;           /**< RDX寄存器(参数3) */
    uint_t r_rcx;           /**< RCX寄存器(参数4) */
    uint_t r_rbx;           /**< RBX寄存器(callee-saved) */
    uint_t r_rax;           /**< RAX寄存器(返回值) */
    /* CPU自动压入 */
    uint_t r_rip_old;       /**< 中断返回地址(RIP) */
    uint_t r_cs_old;        /**< 中断前代码段(CS) */
    uint_t r_rflags;         /**< 中断前标志寄存器(RFLAGS) */
    uint_t r_rsp_old;       /**< 中断前栈指针(RSP) */
    uint_t r_ss_old;        /**< 中断前栈段(SS) */
} intstkregs_t;

/**
 * @brief 异常栈帧结构(带错误码)
 * 
 * 用于页错误、通用保护错误等会压入错误码的异常
 * 结构与intstkregs_t类似，但多一个错误码字段
 */
typedef struct s_FAULTSTKREGS {
    /* 软件保存的段寄存器 */
    uint_t r_gs;            /**< GS段寄存器 */
    uint_t r_fs;            /**< FS段寄存器 */
    uint_t r_es;            /**< ES段寄存器 */
    uint_t r_ds;            /**< DS段寄存器 */
    /* 软件保存的通用寄存器 */
    uint_t r_r15;           /**< R15寄存器 */
    uint_t r_r14;           /**< R14寄存器 */
    uint_t r_r13;           /**< R13寄存器 */
    uint_t r_r12;           /**< R12寄存器 */
    uint_t r_r11;           /**< R11寄存器 */
    uint_t r_r10;           /**< R10寄存器 */
    uint_t r_r9;            /**< R9寄存器 */
    uint_t r_r8;            /**< R8寄存器 */
    uint_t r_rdi;           /**< RDI寄存器 */
    uint_t r_rsi;           /**< RSI寄存器 */
    uint_t r_rbp;           /**< RBP寄存器 */
    uint_t r_rdx;           /**< RDX寄存器 */
    uint_t r_rcx;           /**< RCX寄存器 */
    uint_t r_rbx;           /**< RBX寄存器 */
    uint_t r_rax;           /**< RAX寄存器 */
    /* CPU自动压入 */
    uint_t r_errcode;       /**< 错误码(异常类型相关) */
    uint_t r_rip_old;       /**< 异常返回地址 */
    uint_t r_cs_old;        /**< 异常前代码段 */
    uint_t r_rflags;         /**< 异常前标志寄存器 */
    uint_t r_rsp_old;       /**< 异常前栈指针 */
    uint_t r_ss_old;        /**< 异常前栈段 */
} faultstkregs_t;

#endif

#endif // ARCH_X86_CPU_T_H
