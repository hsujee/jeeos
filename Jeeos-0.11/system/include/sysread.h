/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - read读取操作
 *
 * 提供文件/设备读取的系统调用接口（桩实现）
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_READ_H
#define _SYS_READ_H

/**
 * @brief 系统调用表read入口
 * @param inr 系统调用号
 * @param stkparv 栈参数
 * @return 系统调用状态码
 */
sysstus_t syscall_read(uint_t inr, stkparame_t* stkparv);

/**
 * @brief 读取数据
 * @param fhand 文件/设备句柄
 * @param buf 缓冲区
 * @param len 读取长度
 * @return 实际读取的字节数
 */
size_t sys_read(hand_t fhand, void *buf, size_t len);

/**
 * @brief 读取数据核心函数
 * @param fhand 文件/设备句柄
 * @param buf 缓冲区
 * @param len 读取长度
 * @return 实际读取的字节数
 */
size_t sys_core_read(hand_t fhand, void *buf, size_t len);

/**
 * @brief 设备读取操作
 * @param ondep 对象节点指针
 * @return 系统调用状态码
 */
sysstus_t sys_read_device(objnode_t* ondep);

#endif // _SYS_READ_H
