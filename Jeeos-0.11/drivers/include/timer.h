/*
 * @Author: Jee Hsu
 * @Description: 系统时钟驱动接口
 *
 * 8254 PIT定时器驱动的接口声明：
 * - systick_set_driver: 配置驱动
 * - systick_set_device: 配置设备
 * - systick_handle: 时钟中断处理
 * - 标准驱动接口(open/close/read/write等)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef DRVTICK_H
#define DRVTICK_H
void systick_set_driver(driver_t* drvp);
void systick_set_device(device_t* devp,driver_t* drvp);
drvstus_t systick_entry(driver_t* drvp,uint_t val,void* p);
drvstus_t systick_exit(driver_t* drvp,uint_t val,void* p);
drvstus_t systick_handle(uint_t ift_nr,void* devp,void* sframe);
drvstus_t systick_open(device_t* devp,void* iopack);
drvstus_t systick_close(device_t* devp,void* iopack);
drvstus_t systick_read(device_t* devp,void* iopack);
drvstus_t systick_write(device_t* devp,void* iopack);
drvstus_t systick_lseek(device_t* devp,void* iopack);
drvstus_t systick_ioctrl(device_t* devp,void* iopack);
drvstus_t systick_dev_start(device_t* devp,void* iopack);
drvstus_t systick_dev_stop(device_t* devp,void* iopack);
drvstus_t systick_set_powerstus(device_t* devp,void* iopack);
drvstus_t systick_enum_dev(device_t* devp,void* iopack);
drvstus_t systick_flush(device_t* devp,void* iopack);
drvstus_t systick_shutdown(device_t* devp,void* iopack);
#endif
