/*
 * @Author: Jee Hsu
 * @Description: VGA 文本模式驱动接口声明
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _VGA_H
#define _VGA_H

#include "drv_deps.h"

/* 驱动入口/退出 */
PUBLIC drvstus_t vga_entry(driver_t *drvp, uint_t val, void *p);
PUBLIC drvstus_t vga_exit(driver_t *drvp, uint_t val, void *p);

/* 驱动接口函数 */
PUBLIC drvstus_t vga_open(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_close(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_read(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_write(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_lseek(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_ioctrl(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_dev_start(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_dev_stop(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_set_powerstus(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_enum_dev(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_flush(device_t *devp, void *iopack);
PUBLIC drvstus_t vga_shutdown(device_t *devp, void *iopack);

#endif // _VGA_H

