/*
 * @Author: Jee Hsu
 * @Description: 架构层全局变量声明
 *
 * 声明x86架构使用的全局变量：
 * - GDT/IDT描述符表
 * - TSS任务状态段
 * - 引导参数
 * - 图形设备
 * - 内存管理对象
 * - 中断描述符
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_ARCH_GLOBAL_H
#define ARCH_X86_ARCH_GLOBAL_H

#ifdef	ARCHGOBAL_HEAD
#undef	EXTERN
#define EXTERN
#endif

#ifdef CFG_X86_PLATFORM

/*===========================================================================
 * 异常处理入口声明 (定义在entry.asm)
 *===========================================================================*/

void exc_divide_error();
void exc_single_step_exception();
void exc_nmi();
void exc_breakpoint_exception();
void exc_overflow();
void exc_bounds_check();
void exc_inval_opcode();
void exc_copr_not_available();
void exc_double_fault();
void exc_copr_seg_overrun();
void exc_inval_tss();
void exc_segment_not_present();
void exc_stack_exception();
void exc_general_protection();
void exc_page_fault();
void exc_copr_error();
void exc_alignment_check();
void exc_machine_check();
void exc_simd_fault();
void hxi_exc_general_intpfault();

/*===========================================================================
 * 硬件中断入口声明
 *===========================================================================*/

void hxi_hwint00();
void hxi_hwint01();
void hxi_hwint02();
void hxi_hwint03();
void hxi_hwint04();
void hxi_hwint05();
void hxi_hwint06();
void hxi_hwint07();
void hxi_hwint08();
void hxi_hwint09();
void hxi_hwint10();
void hxi_hwint11();
void hxi_hwint12();
void hxi_hwint13();
void hxi_hwint14();
void hxi_hwint15();
void hxi_hwint16();
void hxi_hwint17();
void hxi_hwint18();
void hxi_hwint19();
void hxi_hwint20();
void hxi_hwint21();
void hxi_hwint22();
void hxi_hwint23();

/*===========================================================================
 * APIC中断入口声明
 *===========================================================================*/

void hxi_apic_svr();
void hxi_apic_ipi_schedule();
void hxi_apic_timer();
void hxi_apic_thermal();
void hxi_apic_perf_monitor();
void hxi_apic_lint0();
void hxi_apic_lint1();
void hxi_apic_error();

/*===========================================================================
 * 系统调用入口声明
 *===========================================================================*/

void exit_syscall();
void asm_ret_from_user_mode();

/*===========================================================================
 * 全局变量声明
 *===========================================================================*/

/** GDT描述符表 - 每个CPU核心一个 */
ARCH_DEFGLOB_VARIABLE(descriptor_t, x64_gdt)[CPUCORE_MAX][GDTMAX];

/** IDT中断描述符表 */
ARCH_DEFGLOB_VARIABLE(gate_t, x64_idt)[IDTMAX];

/** TSS任务状态段 - 每个CPU核心一个 */
ARCH_DEFGLOB_VARIABLE(x64tss_t, x64tss)[CPUCORE_MAX];

/** GDTR寄存器值 - 每个CPU核心一个 */
ARCH_DEFGLOB_VARIABLE(igdtr_t, x64_igdt_reg)[CPUCORE_MAX];

/** IDTR寄存器值 */
ARCH_DEFGLOB_VARIABLE(iidtr_t, x64_iidt_reg);

/** 引导参数结构 */
ARCH_DEFGLOB_VARIABLE(abootparam_t, kabootparam);

/** 默认图形设备 */
ARCH_DEFGLOB_VARIABLE(dftgraph_t, kdftgh);

/** 内存管理对象 */
ARCH_DEFGLOB_VARIABLE(memmgrob_t, memmgrob);

/** 中断描述符数组 */
ARCH_DEFGLOB_VARIABLE(int_desc_t, machintflt)[IDTMAX];

#endif // CFG_X86_PLATFORM

/**
 * @brief 系统停机函数
 * @param dt 停机类型 (0=正常停机)
 */
void die(u32_t dt);

#endif // ARCH_X86_ARCH_GLOBAL_H
