/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - write写入操作
 *
 * 提供文件/设备写入的系统调用接口（桩实现）
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_WRITE_H
#define _SYS_WRITE_H

/**
 * @brief 系统调用表write入口
 * @param inr 系统调用号
 * @param stkparv 栈参数
 * @return 系统调用状态码
 */
sysstus_t syscall_write(uint_t inr, stkparame_t* stkparv);

/**
 * @brief 写入数据
 * @param fhand 文件/设备句柄
 * @param buf 缓冲区
 * @param len 写入长度
 * @return 实际写入的字节数
 */
size_t sys_write(hand_t fhand, void *buf, size_t len);

/**
 * @brief 写入数据核心函数
 * @param fhand 文件/设备句柄
 * @param buf 缓冲区
 * @param len 写入长度
 * @return 实际写入的字节数
 */
size_t sys_core_write(hand_t fhand, void *buf, size_t len);

/**
 * @brief 设备写入操作
 * @param ondep 对象节点指针
 * @return 系统调用状态码
 */
sysstus_t sys_write_device(objnode_t* ondep);

#endif // _SYS_WRITE_H
