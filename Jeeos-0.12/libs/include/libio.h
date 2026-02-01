/*
 * @Author: Jee Hsu
 * @Description: 用户态库 - 文件I/O接口声明
 *
 * 提供文件和设备的I/O操作接口：
 * - open/close: 打开/关闭文件或设备
 * - read/write: 读写数据
 * - lseek: 文件定位
 * - ioctrl: IO控制
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _LIBIO_H
#define _LIBIO_H

#include "libtypes.h"

/* 文件I/O操作 */
hand_t    open(void* file, uint_t flags, uint_t stus);
sysstus_t close(hand_t fhand);
sysstus_t read(hand_t fhand, buf_t buf, size_t len, uint_t flags);
sysstus_t write(hand_t fhand, buf_t buf, size_t len, uint_t flags);
sysstus_t lseek(hand_t fhand, uint_t lofset, uint_t flags);
sysstus_t ioctrl(hand_t fhand, buf_t buf, uint_t iocode, uint_t flags);

#endif /* _LIBIO_H */

