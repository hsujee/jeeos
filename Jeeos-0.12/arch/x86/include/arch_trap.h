/*
 * @Author: Jee Hsu
 * @Description: 架构层中断管理 - 中断描述符和处理
 *
 * 负责x86平台的中断管理，包括：
 * - 中断描述符表(IDT)管理
 * - 中断处理函数注册
 * - 中断线的启用和禁用
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_ARCH_TRAP_H
#define ARCH_X86_ARCH_TRAP_H

#ifdef CFG_X86_PLATFORM

/**
 * @brief 初始化中断描述符结构
 * @param initp 中断描述符结构指针
 * @param flg 中断标志
 * @param sts 中断状态
 * @param prity 中断优先级
 * @param irq 中断号
 */
void int_desc_init(int_desc_t* initp, u32_t flg, u32_t sts, uint_t prity, uint_t irq);

/**
 * @brief 初始化所有中断描述符
 */
void init_int_desc();

/**
 * @brief 架构层中断初始化入口
 * 
 * 初始化GDT、IDT、中断描述符和8259 PIC
 */
PUBLIC void init_archint();

/**
 * @brief 获取指定中断号的中断描述符
 * @param irqnr 中断号
 * @return 中断描述符指针，失败返回NULL
 */
PUBLIC int_desc_t* arch_retn_intfltdsc(uint_t irqnr);

/**
 * @brief 初始化中断服务结构
 * @param initp 中断服务结构指针
 * @param flg 服务标志
 * @param intfltp 关联的中断描述符
 * @param device 关联的设备
 * @param handle 中断处理函数
 */
void int_server_init(int_server_t* initp, u32_t flg, int_desc_t* intfltp, void* device, int_handler_t handle);

/**
 * @brief 添加中断处理函数
 * @param intdscp 中断描述符指针
 * @param serdscp 中断服务结构指针
 * @return TRUE成功，FALSE失败
 */
bool_t arch_add_ihandle(int_desc_t* intdscp, int_server_t* serdscp);

/**
 * @brief 启用指定中断线
 * @param ifdnr 中断线号
 * @return 驱动状态码
 */
drvstus_t arch_enable_intline(uint_t ifdnr);

/**
 * @brief 禁用指定中断线
 * @param ifdnr 中断线号
 * @return 驱动状态码
 */
drvstus_t arch_disable_intline(uint_t ifdnr);

/**
 * @brief 默认中断处理函数
 * @param ift_nr 中断号
 * @param sframe 栈帧指针
 * @return 驱动状态码
 */
drvstus_t arch_intflt_default(uint_t ift_nr, void* sframe);

#endif // CFG_X86_PLATFORM
#endif // ARCH_X86_ARCH_TRAP_H
