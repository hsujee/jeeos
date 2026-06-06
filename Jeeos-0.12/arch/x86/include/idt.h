/*
 * @Author: Jee Hsu
 * @Description: IDT中断描述符表管理 - 异常和中断处理入口
 *
 * 定义x86平台的所有异常处理入口和硬件中断入口：
 * - CPU异常处理（除零、页错误等）
 * - 硬件中断处理（8259 PIC中断）
 * - APIC中断处理
 * - 系统调用入口
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_IDT_H
#define ARCH_X86_IDT_H

/*===========================================================================
 * CPU异常处理入口 (中断向量 0-31)
 *===========================================================================*/

/** @brief 除零错误异常 (向量0) */
void exc_divide_error();

/** @brief 单步调试异常 (向量1) */
void exc_single_step_exception();

/** @brief 不可屏蔽中断 (向量2) */
void exc_nmi();

/** @brief 断点异常 (向量3) */
void exc_breakpoint_exception();

/** @brief 溢出异常 (向量4) */
void exc_overflow();

/** @brief 边界检查异常 (向量5) */
void exc_bounds_check();

/** @brief 无效操作码异常 (向量6) */
void exc_inval_opcode();

/** @brief 协处理器不可用异常 (向量7) */
void exc_copr_not_available();

/** @brief 双重错误异常 (向量8) */
void exc_double_fault();

/** @brief 协处理器段越界 (向量9) */
void exc_copr_seg_overrun();

/** @brief 无效TSS异常 (向量10) */
void exc_inval_tss();

/** @brief 段不存在异常 (向量11) */
void exc_segment_not_present();

/** @brief 栈段异常 (向量12) */
void exc_stack_exception();

/** @brief 一般保护异常 (向量13) */
void exc_general_protection();

/** @brief 页错误异常 (向量14) */
void exc_page_fault();

/** @brief 协处理器错误 (向量16) */
void exc_copr_error();

/** @brief 对齐检查异常 (向量17) */
void exc_alignment_check();

/** @brief 机器检查异常 (向量18) */
void exc_machine_check();

/** @brief SIMD浮点异常 (向量19) */
void exc_simd_fault();

/** @brief 通用中断/异常处理入口 */
void hxi_exc_general_intpfault();

/*===========================================================================
 * 硬件中断处理入口 (8259 PIC, 中断向量 32-55)
 *===========================================================================*/

/** @brief 硬件中断0 - 定时器 */
void hxi_hwint00();
/** @brief 硬件中断1 - 键盘 */
void hxi_hwint01();
/** @brief 硬件中断2 - 级联 */
void hxi_hwint02();
/** @brief 硬件中断3 - 串口COM2 */
void hxi_hwint03();
/** @brief 硬件中断4 - 串口COM1 */
void hxi_hwint04();
/** @brief 硬件中断5 - LPT2 */
void hxi_hwint05();
/** @brief 硬件中断6 - 软盘 */
void hxi_hwint06();
/** @brief 硬件中断7 - LPT1 */
void hxi_hwint07();
/** @brief 硬件中断8 - RTC */
void hxi_hwint08();
/** @brief 硬件中断9 */
void hxi_hwint09();
/** @brief 硬件中断10 */
void hxi_hwint10();
/** @brief 硬件中断11 */
void hxi_hwint11();
/** @brief 硬件中断12 - PS/2鼠标 */
void hxi_hwint12();
/** @brief 硬件中断13 - FPU */
void hxi_hwint13();
/** @brief 硬件中断14 - IDE主 */
void hxi_hwint14();
/** @brief 硬件中断15 - IDE从 */
void hxi_hwint15();
/** @brief 硬件中断16-23 */
void hxi_hwint16();
void hxi_hwint17();
void hxi_hwint18();
void hxi_hwint19();
void hxi_hwint20();
void hxi_hwint21();
void hxi_hwint22();
void hxi_hwint23();

/*===========================================================================
 * APIC中断处理入口
 *===========================================================================*/

/** @brief APIC伪中断处理 */
void hxi_apic_svr();
/** @brief APIC IPI调度中断 */
void hxi_apic_ipi_schedule();
/** @brief APIC定时器中断 */
void hxi_apic_timer();
/** @brief APIC热量监控中断 */
void hxi_apic_thermal();
/** @brief APIC性能监控中断 */
void hxi_apic_perf_monitor();
/** @brief APIC LINT0中断 */
void hxi_apic_lint0();
/** @brief APIC LINT1中断 */
void hxi_apic_lint1();
/** @brief APIC错误中断 */
void hxi_apic_error();

/*===========================================================================
 * 系统调用和返回
 *===========================================================================*/

/** @brief 系统调用入口 */
void exit_syscall();
/** @brief 从用户模式返回 */
void _ret_from_user_mode();

/*===========================================================================
 * IDT初始化函数
 *===========================================================================*/

/**
 * @brief 初始化GDT描述符
 */
PUBLIC void init_descriptor();

/**
 * @brief 初始化IDT描述符表
 */
PUBLIC void init_idt_descriptor();

/**
 * @brief 设置IDT描述符
 * @param vector 中断向量号
 * @param desc_type 描述符类型
 * @param handler 中断处理函数
 * @param privilege 特权级(0-3)
 */
PUBLIC void set_idt_desc(u8_t vector, u8_t desc_type, inthandler_t handler, u8_t privilege);

#endif // ARCH_X86_IDT_H
