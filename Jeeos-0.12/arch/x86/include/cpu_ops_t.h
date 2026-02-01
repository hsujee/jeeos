/*
 * @Author: Jee Hsu
 * @Description: CPU操作类型定义 - CPU模式和中断标志常量
 *
 * 定义ARM/x86 CPU模式常量和中断标志位
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_CPU_OPS_T_H
#define ARCH_X86_CPU_OPS_T_H

/*===========================================================================
 * ARM CPU模式定义 (保留以便移植)
 *===========================================================================*/

#define CPU_USR_MODE 0x10   /**< 用户模式 */
#define CPU_FIQ_MODE 0x11   /**< 快速中断模式 */
#define CPU_IRQ_MODE 0x12   /**< 普通中断模式 */
#define CPU_SVE_MODE 0x13   /**< 管理模式 */
#define CPU_ABT_MODE 0x17   /**< 中止模式 */
#define CPU_UND_MODE 0x1b   /**< 未定义模式 */
#define CPU_SYS_MODE 0x1f   /**< 系统模式 */

/*===========================================================================
 * 中断标志位定义
 *===========================================================================*/

#define CFIQ 0x40           /**< FIQ禁用位 */
#define CIRQ 0x80           /**< IRQ禁用位 */
#define CIRQFIQ 0xc0        /**< IRQ和FIQ禁用位 */

#endif // ARCH_X86_CPU_OPS_T_H
