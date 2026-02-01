/*
 * @Author: Jee Hsu
 * @Description: 系统调用分发入口
 *
 * 接收来自用户态的系统调用请求并分发：
 * - 验证系统调用号
 * - 提取参数
 * - 调用对应的处理函数
 * - 返回结果
 *
 * 系统调用流程：
 * 用户态 -> int 0xFF -> arch_syscall_allocator -> syshandle -> 具体处理
 *
 * 当前状态: 桩实现(待完善)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

// 系统调用处理函数 - 桩实现
sysstus_t syshandle(uint_t inr, void* sframe) {
    // TODO: 0.12中实现完整的系统调用分发
    (void)inr;
    (void)sframe;
    return SYSSTUSERR;
}
