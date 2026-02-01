/*
 * @Author: Jee Hsu
 * @Description: x86-64 GDT/IDT/TSS描述符类型定义
 *
 * 定义x86-64保护模式的核心数据结构：
 * - descriptor_t: 段描述符(GDT表项)
 * - gate_t: 门描述符(IDT表项)
 * - x64tss_t: 64位任务状态段
 * - igdtr_t/iidtr_t: GDTR/IDTR寄存器
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_IDT_T_H
#define ARCH_X86_IDT_T_H

/** @name 表项数量限制 */
/** @{ */
#define DEVCLASSMAX 256     /**< 设备类最大数量 */
#define GDTMAX 10           /**< GDT最大表项数 */
#define IDTMAX 256          /**< IDT最大表项数 */
#define STACKMAX 0x400      /**< 栈最大大小 */
/** @} */

#define K_CS_IDX	0x08
#define K_DS_IDX	0x10
#define U_CS_IDX	0x1b
#define U_DS_IDX	0x23
#define K_TAR_IDX	0x28
#define UMOD_EFLAGS	0x1202

/* GDT */
/* 描述符索引 */
#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3

#define	INDEX_DUMMY			0
#define	INDEX_FLAT_C		1
#define	INDEX_FLAT_RW		2
/* 选择子 */
#define	SELECTOR_DUMMY		0		
#define	SELECTOR_FLAT_C		0x08			
#define	SELECTOR_FLAT_RW	0x10		 

#define	SELECTOR_KERNEL_CS	SELECTOR_FLAT_C
#define	SELECTOR_KERNEL_DS	SELECTOR_FLAT_RW

/* 描述符类型值说明 */
#define DA_64 			0x2000
#define	DA_32			0x4000	/* 32 位段				*/
#define	DA_LIMIT_4K		0x8000	/* 段界限粒度为 4K 字节			*/
#define	DA_DPL0			0x00	/* DPL = 0				*/
#define	DA_DPL1			0x20	/* DPL = 1				*/
#define	DA_DPL2			0x40	/* DPL = 2				*/
#define	DA_DPL3			0x60	/* DPL = 3				*/
/* 存储段描述符类型值说明 */
#define	DA_DR			0x90	/* 存在的只读数据段类型值		*/
#define	DA_DRW			0x92	/* 存在的可读写数据段属性值		*/
#define	DA_DRWA			0x93	/* 存在的已访问可读写数据段类型值	*/
#define	DA_C			0x98	/* 存在的只执行代码段属性值		*/
#define	DA_CR			0x9A	/* 存在的可执行可读代码段属性值		*/
#define	DA_CCO			0x9C	/* 存在的只执行一致代码段属性值		*/
#define	DA_CCOR			0x9E	/* 存在的可执行可读一致代码段属性值	*/
/* 系统段描述符类型值说明 */
#define	DA_LDT			0x82	/* 局部描述符表段类型值			*/
#define	DA_TaskGate		0x85	/* 任务门类型值				*/
#define	DA_386TSS		0x89	/* 可用 386 任务状态段类型值		*/
#define	DA_386CGate		0x8C	/* 386 调用门类型值			*/
#define	DA_386IGate		0x8E	/* 386 中断门类型值			*/
#define	DA_386TGate		0x8F	/* 386 陷阱门类型值			*/

/* 中断向量 */
#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT		0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT	0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT		0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10
#define	INT_VECTOR_ALIGN_CHEK		0x11
#define	INT_VECTOR_MACHI_CHEK		0x12
#define	INT_VECTOR_SIMD_FAULT		0x13

/* 中断向量 */
#define	INT_VECTOR_IRQ0			0x20
#define	INT_VECTOR_IRQ8			0x28

#define INT_VECTOR_APIC_IPI_SCHEDUL     0xe0
#define	INT_VECTOR_APIC_SVR		0xef
#define INT_VECTOR_APIC_TIMER		0xf0
#define INT_VECTOR_APIC_THERMAL		0xf1
#define INT_VECTOR_APIC_PERFORM		0xf2
#define INT_VECTOR_APIC_LINTO		0xf3
#define INT_VECTOR_APIC_LINTI		0xf4
#define INT_VECTOR_APIC_ERROR		0xf5

#define INT_VECTOR_SYSCALL		0xFF

/**
 * @brief GDT段描述符结构(8字节)
 * 
 * 定义内存段的基地址、界限和属性
 * 64位模式下主要用于定义代码段和数据段的属性
 */
typedef struct s_descriptor {
    u16_t limit_low;        /**< 段界限[15:0] */
    u16_t base_low;         /**< 段基址[15:0] */
    u8_t base_mid;          /**< 段基址[23:16] */
    u8_t attr1;             /**< 属性: P(1) DPL(2) S(1) TYPE(4) */
    u8_t limit_high_attr2;  /**< 属性: G(1) D/B(1) L(1) AVL(1) 界限[19:16] */
    u8_t base_high;         /**< 段基址[31:24] */
} __attribute__((packed)) descriptor_t;

/**
 * @brief IDT门描述符结构(16字节, x86-64)
 * 
 * 用于中断门、陷阱门、调用门的定义
 * 包含中断处理函数的入口地址和属性
 */
typedef struct s_GATE {
    u16_t offset_low;       /**< 偏移地址[15:0] */
    u16_t selector;         /**< 目标代码段选择子 */
    u8_t dcount;            /**< IST索引(64位)或参数计数(32位调用门) */
    u8_t attr;              /**< 属性: P(1) DPL(2) 0(1) TYPE(4) */
    u16_t offset_high;      /**< 偏移地址[31:16] */
    u32_t offset_high_h;    /**< 偏移地址[63:32] (64位扩展) */
    u32_t offset_resv;      /**< 保留(必须为0) */
} __attribute__((packed)) gate_t;

/**
 * @brief GDTR寄存器结构(10字节)
 * 
 * 用于LGDT指令加载GDT表
 */
typedef struct s_GDTPTR {
    u16_t gdtLen;           /**< GDT表界限(字节数-1) */
    u64_t gdt_base;         /**< GDT表基地址 */
} __attribute__((packed)) igdtr_t;

/**
 * @brief IDTR寄存器结构(10字节)
 * 
 * 用于LIDT指令加载IDT表
 */
typedef struct s_IDTPTR {
    u16_t idtLen;           /**< IDT表界限(字节数-1) */
    u64_t idt_base;         /**< IDT表基地址 */
} __attribute__((packed)) iidtr_t;

/**
 * @brief x86-64任务状态段结构(104字节)
 * 
 * 64位模式下TSS主要用于保存特权级栈指针(RSP0/1/2)
 * 当发生特权级切换时，CPU从TSS加载目标特权级的栈指针
 */
typedef struct s_X64TSS {
    u32_t reserv0;          /**< 保留(必须为0) */
    u64_t rsp0;             /**< Ring0栈指针(用户态->内核态切换) */
    u64_t rsp1;             /**< Ring1栈指针(未使用) */
    u64_t rsp2;             /**< Ring2栈指针(未使用) */
    u64_t reserv28;         /**< 保留 */
    u64_t ist[7];           /**< 中断栈表IST1-7(用于特定中断) */
    u64_t reserv92;         /**< 保留 */
    u16_t reserv100;        /**< 保留 */
    u16_t iobase;           /**< I/O权限位图偏移(未使用) */
} __attribute__((packed)) x64tss_t;

#endif // ARCH_X86_IDT_T_H
