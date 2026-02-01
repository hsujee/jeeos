/*
 * @Author: Jee Hsu
 * @Description: PS/2 键盘驱动接口声明
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "drv_deps.h"

/* 驱动入口/退出 */
PUBLIC drvstus_t keyboard_entry(driver_t *drvp, uint_t val, void *p);
PUBLIC drvstus_t keyboard_exit(driver_t *drvp, uint_t val, void *p);

/* 中断处理函数 */
PUBLIC drvstus_t keyboard_handle(uint_t ift_nr, void *devp, void *sframe);

/* 驱动接口函数 */
PUBLIC drvstus_t keyboard_open(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_close(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_read(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_write(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_lseek(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_ioctrl(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_dev_start(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_dev_stop(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_set_powerstus(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_enum_dev(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_flush(device_t *devp, void *iopack);
PUBLIC drvstus_t keyboard_shutdown(device_t *devp, void *iopack);

#endif // _KEYBOARD_H

