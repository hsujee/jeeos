/*
 * @Author: Jee Hsu
 * @Description: 中断管理 - 中断处理函数注册
 *
 * 提供中断处理函数的注册和管理接口
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _INTERRUPT_H
#define _INTERRUPT_H

/**
 * @brief 注册中断处理函数
 * @param device 关联设备指针
 * @param handle 中断处理函数
 * @param phyiline 物理中断线号
 * @return 中断服务结构指针，失败返回NULL
 */
int_server_t* add_irqhandle(void* device, int_handler_t handle, uint_t phyiline);

/**
 * @brief 启用中断线
 * @param ifdnr 中断线号
 * @return 驱动状态码
 */
drvstus_t enable_intline(uint_t ifdnr);

/**
 * @brief 禁用中断线
 * @param ifdnr 中断线号
 * @return 驱动状态码
 */
drvstus_t disable_intline(uint_t ifdnr);

#endif // _INTERRUPT_H
