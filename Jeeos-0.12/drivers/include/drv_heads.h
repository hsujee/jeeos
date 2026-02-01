/*
 * @Author: Jee Hsu
 * @Description: 驱动层头文件聚合
 *
 * 包含所有设备驱动的头文件：
 * - timer.h: 系统时钟驱动
 * - ramfs.h: RAM文件系统驱动
 * - serial.h: UART串口驱动
 * - nullzero.h: NULL/ZERO虚拟设备驱动
 * - speaker.h: PC蜂鸣器驱动
 * - rtc.h: RTC实时时钟驱动
 * - random.h: 随机数设备驱动
 * - vga.h: VGA文本模式驱动
 * - keyboard.h: PS/2键盘驱动
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _DRV_HEADS_H
#define _DRV_HEADS_H
#include "timer.h"
#include "ramfs.h"
#include "serial.h"
#include "nullzero.h"
#include "speaker.h"
#include "rtc.h"
#include "random.h"
#include "vga.h"
#include "keyboard.h"
#endif // _DRV_HEADS_H

