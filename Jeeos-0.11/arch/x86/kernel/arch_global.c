/*
 * @Author: Jee Hsu
 * @Description: x86架构层全局变量定义
 *
 * 定义x86-64架构相关的全局变量：
 * - x64_gdt: 64位全局描述符表(每CPU一份)
 * - x64_idt: 64位中断描述符表
 * - x64tss: 任务状态段(每CPU一份)
 * - kabootparam: 引导参数结构体
 * - kdftgh: 视频驱动结构体
 * - machintflt: 中断处理描述符表
 *
 * 使用ARCH_DEFGLOB_VARIABLE宏将变量放入.data段
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#define ARCHGOBAL_HEAD
#include "global_types.h"
#include "global_heads.h"

#ifdef CFG_X86_PLATFORM
ARCH_DEFGLOB_VARIABLE(descriptor_t,x64_gdt)[CPUCORE_MAX][GDTMAX];
ARCH_DEFGLOB_VARIABLE(gate_t,x64_idt)[IDTMAX];
ARCH_DEFGLOB_VARIABLE(x64tss_t,x64tss)[CPUCORE_MAX]; 
ARCH_DEFGLOB_VARIABLE(igdtr_t,x64_igdt_reg)[CPUCORE_MAX];
ARCH_DEFGLOB_VARIABLE(iidtr_t,x64_iidt_reg);
ARCH_DEFGLOB_VARIABLE(abootparam_t, kabootparam);
ARCH_DEFGLOB_VARIABLE(dftgraph_t, kdftgh);

ARCH_DEFGLOB_VARIABLE(int_desc_t, machintflt)[IDTMAX];
#endif

