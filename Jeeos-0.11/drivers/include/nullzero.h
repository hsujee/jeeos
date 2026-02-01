/*
 * @Author: Jee Hsu
 * @Description: NULL/ZERO 虚拟设备驱动接口
 *
 * 声明 null 和 zero 虚拟设备驱动的接口函数
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _NULLZERO_H
#define _NULLZERO_H

/*===========================================================================
 * 驱动入口/退出
 *===========================================================================*/

drvstus_t nullzero_entry(driver_t *drvp, uint_t val, void *p);
drvstus_t nullzero_exit(driver_t *drvp, uint_t val, void *p);

/*===========================================================================
 * 设备配置
 *===========================================================================*/

void nullzero_set_driver(driver_t *drvp);
void null_set_device(device_t *devp, driver_t *drvp);
void zero_set_device(device_t *devp, driver_t *drvp);

/*===========================================================================
 * 标准驱动接口
 *===========================================================================*/

drvstus_t nullzero_open(device_t *devp, void *iopack);
drvstus_t nullzero_close(device_t *devp, void *iopack);
drvstus_t nullzero_read(device_t *devp, void *iopack);
drvstus_t nullzero_write(device_t *devp, void *iopack);
drvstus_t nullzero_lseek(device_t *devp, void *iopack);
drvstus_t nullzero_ioctrl(device_t *devp, void *iopack);
drvstus_t nullzero_dev_start(device_t *devp, void *iopack);
drvstus_t nullzero_dev_stop(device_t *devp, void *iopack);
drvstus_t nullzero_set_powerstus(device_t *devp, void *iopack);
drvstus_t nullzero_enum_dev(device_t *devp, void *iopack);
drvstus_t nullzero_flush(device_t *devp, void *iopack);
drvstus_t nullzero_shutdown(device_t *devp, void *iopack);

#endif /* _NULLZERO_H */

