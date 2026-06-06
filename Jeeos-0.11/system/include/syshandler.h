/*
 * @Author: Jee Hsu
 * @Description: 系统调用处理器
 *
 * 系统调用入口和分发（桩实现）
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_HANDLER_H
#define _SYS_HANDLER_H

/**
 * @brief 系统调用处理入口
 * @param inr 系统调用号
 * @param sframe 栈帧指针
 * @return 系统调用状态码
 */
sysstus_t syshandle(uint_t inr, void* sframe);

#endif // _SYS_HANDLER_H
