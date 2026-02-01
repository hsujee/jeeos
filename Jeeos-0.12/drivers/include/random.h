/*
 * @Author: Jee Hsu
 * @Description: 随机数设备驱动接口声明
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _RANDOM_H
#define _RANDOM_H

#include "drv_deps.h"

/* 驱动入口/退出 */
PUBLIC drvstus_t random_entry(driver_t *drvp, uint_t val, void *p);
PUBLIC drvstus_t random_exit(driver_t *drvp, uint_t val, void *p);

/* 驱动接口函数 */
PUBLIC drvstus_t random_open(device_t *devp, void *iopack);
PUBLIC drvstus_t random_close(device_t *devp, void *iopack);
PUBLIC drvstus_t random_read(device_t *devp, void *iopack);
PUBLIC drvstus_t random_write(device_t *devp, void *iopack);
PUBLIC drvstus_t random_lseek(device_t *devp, void *iopack);
PUBLIC drvstus_t random_ioctrl(device_t *devp, void *iopack);
PUBLIC drvstus_t random_dev_start(device_t *devp, void *iopack);
PUBLIC drvstus_t random_dev_stop(device_t *devp, void *iopack);
PUBLIC drvstus_t random_set_powerstus(device_t *devp, void *iopack);
PUBLIC drvstus_t random_enum_dev(device_t *devp, void *iopack);
PUBLIC drvstus_t random_flush(device_t *devp, void *iopack);
PUBLIC drvstus_t random_shutdown(device_t *devp, void *iopack);

#endif // _RANDOM_H

