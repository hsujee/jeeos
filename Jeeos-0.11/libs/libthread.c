/*
 * @Author: Jee Hsu
 * @Description: 用户态库 - 线程管理
 *
 * 提供用户态的线程操作函数：
 * - exel(): 执行/创建线程
 * - exit(): 退出当前线程
 * - pid(): 获取线程句柄
 * - retn_threadstats(): 获取线程状态
 * - set_threadstats(): 设置线程状态
 *
 * 底层实现：
 * - 通过系统调用与内核线程子系统交互
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "libtypes.h"
#include "libsyscalls.h"

/**
 * @brief 执行/创建线程
 * @param file 可执行文件路径
 * @param flags 执行标志
 * @return 成功返回线程句柄，失败返回-1
 */
hand_t exel(void* file, uint_t flags) {
    hand_t rethand;
    __SYSCALL_PARAM2(INR_TD_EXEL, rethand, file, flags);
    return rethand;
}

/**
 * @brief 退出当前线程
 */
void exit(void) {
    uint_t rs, pv1 = 0;
    __SYSCALL_PARAM1(INR_TD_EXIT, rs, pv1);
    return;
}

/**
 * @brief 获取线程句柄
 * @param tname 线程名称
 * @return 成功返回线程句柄，失败返回-1
 */
hand_t pid(void* tname) {
    hand_t rethand;
    __SYSCALL_PARAM1(INR_TD_HAND, rethand, tname);
    return rethand;
}

/**
 * @brief 获取线程状态
 * @param thand 线程句柄
 * @param scode 状态码
 * @param data 数据参数
 * @param buf 结果缓冲区
 * @return 成功返回SYSSTUSOK，失败返回SYSSTUSERR
 */
sysstus_t retn_threadstats(hand_t thand, uint_t scode, uint_t data, buf_t buf) {
    sysstus_t rets;
    __SYSCALL_PARAM4(INR_TD_RSTATS, rets, thand, scode, data, buf);
    return rets;
}

/**
 * @brief 设置线程状态
 * @param thand 线程句柄
 * @param scode 状态码
 * @param data 数据参数
 * @param buf 数据缓冲区
 * @return 成功返回SYSSTUSOK，失败返回SYSSTUSERR
 */
sysstus_t set_threadstats(hand_t thand, uint_t scode, uint_t data, buf_t buf) {
    sysstus_t rets;
    __SYSCALL_PARAM4(INR_TD_SSTATS, rets, thand, scode, data, buf);
    return rets;
}

