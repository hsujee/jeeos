/*
 * @Author: Jee Hsu
 * @Description: 8259 PIC中断控制器驱动
 *
 * 管理Intel 8259可编程中断控制器，包括：
 * - 主从8259初始化
 * - 中断线启用/禁用
 * - EOI(中断结束)信号发送
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_I8259_H
#define ARCH_X86_I8259_H

/**
 * @brief 初始化8259 PIC
 * 
 * 配置主从8259控制器，设置中断向量偏移，
 * 默认屏蔽所有中断线
 */
void init_i8259();

/**
 * @brief 发送EOI信号
 * 
 * 向8259发送中断结束信号，允许处理下一个中断
 */
void i8259_send_eoi();

/**
 * @brief 启用指定中断线
 * @param line 中断线号(0-15)
 */
void i8259_enabled_line(u32_t line);

/**
 * @brief 禁用指定中断线
 * @param line 中断线号(0-15)
 */
void i8259_disable_line(u32_t line);

/**
 * @brief 保存并禁用中断线
 * @param svline 保存原状态的指针
 * @param line 中断线号(0-15)
 */
void i8259_save_disableline(u64_t* svline, u32_t line);

/**
 * @brief 恢复并启用中断线
 * @param svline 保存的原状态指针
 * @param line 中断线号(0-15)
 */
void i8259_rest_enabledline(u64_t* svline, u32_t line);

#endif // ARCH_X86_I8259_H
