/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - ioctl设备控制
 *
 * 提供设备控制的系统调用接口（桩实现）
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_IOCTRL_H
#define _SYS_IOCTRL_H

/**
 * @brief 系统调用表ioctrl入口
 * @param inr 系统调用号
 * @param stkparv 栈参数
 * @return 系统调用状态码
 */
sysstus_t syscall_ioctrl(uint_t inr, stkparame_t* stkparv);

/**
 * @brief 设备控制
 * @param fhand 文件/设备句柄
 * @param ctrlcmd 控制命令
 * @param buf 缓冲区
 * @param bufsz 缓冲区大小
 * @return 系统调用状态码
 */
sysstus_t sys_ioctrl(hand_t fhand, uint_t ctrlcmd, void *buf, size_t bufsz);

/**
 * @brief 设备控制核心函数
 * @param fhand 文件/设备句柄
 * @param ctrlcmd 控制命令
 * @param buf 缓冲区
 * @param bufsz 缓冲区大小
 * @return 系统调用状态码
 */
sysstus_t sys_core_ioctrl(hand_t fhand, uint_t ctrlcmd, void *buf, size_t bufsz);

/**
 * @brief 设备ioctl操作
 * @param ondep 对象节点指针
 * @return 系统调用状态码
 */
sysstus_t sys_ioctrl_device(objnode_t* ondep);

#endif // _SYS_IOCTRL_H
