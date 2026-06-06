/*
 * @Author: Jee Hsu
 * @Description: 8259A可编程中断控制器驱动
 *
 * 管理x86系统的传统中断控制器：
 * - 主片(Master): IRQ0-7 -> 向量0x20-0x27
 * - 从片(Slave): IRQ8-15 -> 向量0x28-0x2F
 *
 * 主要接口：
 * - init_i8259(): 初始化8259
 * - i8259_send_eoi(): 发送中断结束信号
 * - i8259_enabled_line(): 启用中断线
 * - i8259_disable_line(): 禁用中断线
 *
 * 常用IRQ分配：
 * - IRQ0: 定时器
 * - IRQ1: 键盘
 * - IRQ4: COM1串口
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

void init_i8259() {
	//初始化主从8259a
	out_u8_p(ZIOPT, ICW1);
	out_u8_p(SIOPT, ICW1);
	out_u8_p(ZIOPT1, ZICW2);
	out_u8_p(SIOPT1, SICW2);
	out_u8_p(ZIOPT1, ZICW3);
	out_u8_p(SIOPT1, SICW3);
	out_u8_p(ZIOPT1, ICW4);
	out_u8_p(SIOPT1, ICW4);

	//由于初始化阶段还不能处理中断，初始化中断控制器后，屏蔽所有的中断源
	out_u8_p(ZIOPT1, 0xff);
	out_u8_p(SIOPT1, 0xff);
	return;
}

void i8259_send_eoi() {
	out_u8_p(_INTM_CTL, _EOI);
	out_u8_p(_INTS_CTL, _EOI);
	return;
}

void i8259_enabled_line(u32_t line) {
	cpuflg_t flags;
	save_flags_cli(&flags);
	if (line < 8) {
		u8_t amtmp = in_u8(_INTM_CTLMASK);
		amtmp &= (u8_t)(~(1 << line));
		out_u8_p(_INTM_CTLMASK, amtmp);
	} else {
		u8_t astmp = in_u8(_INTM_CTLMASK);
		astmp &= (u8_t)(~(1 << 2));
		out_u8_p(_INTM_CTLMASK, astmp);
		astmp = in_u8(_INTS_CTLMASK);
		astmp &= (u8_t)(~(1 << (line - 8)));
		out_u8_p(_INTS_CTLMASK, astmp);
	}
	restore_flags_sti(&flags);
	return;
}

void i8259_disable_line(u32_t line) {
	cpuflg_t flags;
	save_flags_cli(&flags);
	if (line < 8) {
		u8_t amtmp = in_u8(_INTM_CTLMASK);
		amtmp |= (u8_t)((1 << line));
		out_u8_p(_INTM_CTLMASK, amtmp);
	} else {
		u8_t astmp = in_u8(_INTM_CTLMASK);
		astmp |= (u8_t)((1 << 2));
		out_u8_p(_INTM_CTLMASK, astmp);
		astmp = in_u8(_INTS_CTLMASK);
		astmp |= (u8_t)((1 << (line - 8)));
		out_u8_p(_INTS_CTLMASK, astmp);
	}
	restore_flags_sti(&flags);
	return;
}

void i8259_save_disableline(u64_t *svline, u32_t line) {
	u32_t intftmp;
	cpuflg_t flags;
	save_flags_cli(&flags);
	u8_t altmp = in_u8(_INTM_CTLMASK);
	intftmp = altmp;
	altmp = in_u8(_INTS_CTLMASK);
	intftmp = (intftmp << 8) | altmp;
	*svline = intftmp;
	i8259_disable_line(line);

	restore_flags_sti(&flags);
	return;
}

void i8259_rest_enabledline(u64_t *svline, u32_t line) {
	cpuflg_t flags;
	save_flags_cli(&flags);

	u32_t intftmp = (u32_t)(*svline);

	u8_t altmp = (intftmp & 0xff);
	out_u8_p(_INTS_CTLMASK, altmp);
	altmp = (u8_t)(intftmp >> 8);
	out_u8_p(_INTM_CTLMASK, altmp);

	restore_flags_sti(&flags);

	return;
}
