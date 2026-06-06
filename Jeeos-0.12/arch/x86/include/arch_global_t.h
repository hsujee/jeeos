/*
 * @Author: Jee Hsu
 * @Description: 全局变量定义
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_ARCH_GLOBAL_T_H
#define ARCH_X86_ARCH_GLOBAL_T_H

#define ARCH_DEFGLOB_VARIABLE(vartype,varname) \
EXTERN  __attribute__((section(".data"))) vartype varname

#endif // ARCH_X86_ARCH_GLOBAL_T_H
