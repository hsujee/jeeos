/*
 * @Author: Jee Hsu
 * @Description: 用户态库 - 文件I/O操作
 *
 * 统一的文件I/O接口实现：
 * - open(): 打开文件/设备
 * - close(): 关闭文件/设备
 * - read(): 读取数据
 * - write(): 写入数据
 * - lseek(): 文件定位
 * - ioctrl(): IO控制
 *
 * 底层实现：
 * - 通过系统调用宏直接触发软中断
 * - 传递参数到内核进行处理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "libtypes.h"
#include "libsyscalls.h"

/**
 * @brief 打开文件或设备
 * @param file 文件路径或设备描述符
 * @param flags 打开标志 (RW_FLG, RO_FLG, WO_FLG 等)
 * @param stus 状态/模式
 * @return 成功返回文件句柄，失败返回-1
 */
hand_t open(void* file, uint_t flags, uint_t stus) {
    hand_t rethand;
    __SYSCALL_PARAM3(INR_FS_OPEN, rethand, file, flags, stus);
    return rethand;
}

/**
 * @brief 关闭文件或设备
 * @param fhand 文件句柄
 * @return 成功返回SYSSTUSOK，失败返回SYSSTUSERR
 */
sysstus_t close(hand_t fhand) {
    sysstus_t rets;
    __SYSCALL_PARAM1(INR_FS_CLOSE, rets, fhand);
    return rets;
}

/**
 * @brief 从文件或设备读取数据
 * @param fhand 文件句柄
 * @param buf 目标缓冲区
 * @param len 读取长度
 * @param flags 读取标志
 * @return 成功返回读取字节数，失败返回SYSSTUSERR
 */
sysstus_t read(hand_t fhand, buf_t buf, size_t len, uint_t flags) {
    sysstus_t rets;
    __SYSCALL_PARAM4(INR_FS_READ, rets, fhand, buf, len, flags);
    return rets;
}

/**
 * @brief 向文件或设备写入数据
 * @param fhand 文件句柄
 * @param buf 源数据缓冲区
 * @param len 写入长度
 * @param flags 写入标志
 * @return 成功返回写入字节数，失败返回SYSSTUSERR
 */
sysstus_t write(hand_t fhand, buf_t buf, size_t len, uint_t flags) {
    sysstus_t rets;
    __SYSCALL_PARAM4(INR_FS_WRITE, rets, fhand, buf, len, flags);
    return rets;
}

/**
 * @brief 文件定位
 * @param fhand 文件句柄
 * @param lofset 偏移量
 * @param flags 定位标志
 * @return 成功返回新位置，失败返回SYSSTUSERR
 */
sysstus_t lseek(hand_t fhand, uint_t lofset, uint_t flags) {
    sysstus_t rets;
    __SYSCALL_PARAM3(INR_FS_LSEEK, rets, fhand, lofset, flags);
    return rets;
}

/**
 * @brief IO控制操作
 * @param fhand 文件句柄
 * @param buf 参数缓冲区
 * @param iocode IO控制码
 * @param flags 控制标志
 * @return 成功返回SYSSTUSOK，失败返回SYSSTUSERR
 */
sysstus_t ioctrl(hand_t fhand, buf_t buf, uint_t iocode, uint_t flags) {
    sysstus_t rets;
    __SYSCALL_PARAM4(INR_FS_IOCTRL, rets, fhand, buf, iocode, flags);
    return rets;
}

