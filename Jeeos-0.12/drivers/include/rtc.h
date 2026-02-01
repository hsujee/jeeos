/*
 * @Author: Jee Hsu
 * @Description: RTC 实时时钟驱动接口声明
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _RTC_H
#define _RTC_H

#include "drv_deps.h"

/* 驱动入口/退出 */
PUBLIC drvstus_t rtc_entry(driver_t *drvp, uint_t val, void *p);
PUBLIC drvstus_t rtc_exit(driver_t *drvp, uint_t val, void *p);

/* 驱动接口函数 */
PUBLIC drvstus_t rtc_open(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_close(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_read(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_write(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_lseek(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_ioctrl(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_dev_start(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_dev_stop(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_set_powerstus(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_enum_dev(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_flush(device_t *devp, void *iopack);
PUBLIC drvstus_t rtc_shutdown(device_t *devp, void *iopack);

#endif // _RTC_H

