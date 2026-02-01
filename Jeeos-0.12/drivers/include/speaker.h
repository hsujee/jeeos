/*
 * @Author: Jee Hsu
 * @Description: PC Speaker 蜂鸣器驱动接口声明
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SPEAKER_H
#define _SPEAKER_H

#include "drv_deps.h"

/* 驱动入口/退出 */
PUBLIC drvstus_t speaker_entry(driver_t *drvp, uint_t val, void *p);
PUBLIC drvstus_t speaker_exit(driver_t *drvp, uint_t val, void *p);

/* 驱动接口函数 */
PUBLIC drvstus_t speaker_open(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_close(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_read(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_write(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_lseek(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_ioctrl(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_dev_start(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_dev_stop(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_set_powerstus(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_enum_dev(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_flush(device_t *devp, void *iopack);
PUBLIC drvstus_t speaker_shutdown(device_t *devp, void *iopack);

#endif // _SPEAKER_H

