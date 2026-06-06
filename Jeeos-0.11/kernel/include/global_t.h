/*
 * @Author: Jee Hsu
 * @Description: 全局变量定义
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _GLOBAL_T_H
#define _GLOBAL_T_H
#define KRL_DEFGLOB_VARIABLE(vartype,varname) \
KEXTERN  __attribute__((section(".data"))) vartype varname
#endif // GLOBAL_T_H
