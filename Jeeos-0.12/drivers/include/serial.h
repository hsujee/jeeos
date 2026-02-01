/*
 * @Author: Jee Hsu
 * @Description: UART串口驱动接口
 *
 * 16550兼容串口驱动的接口声明：
 * - uart_set_driver: 配置驱动
 * - uart0_set_device: 配置COM1设备
 * - 标准驱动接口(open/close/read/write等)
 *
 * 支持的串口：
 * - COM1 (0x3F8)
 * - COM2 (0x2F8)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef DRVUART_H
#define DRVUART_H
void uart_set_driver(driver_t* drvp);
void uart0_set_device(device_t* devp,driver_t* drvp);
drvstus_t uart_entry(driver_t* drvp,uint_t val,void* p);
drvstus_t uart_exit(driver_t* drvp,uint_t val,void* p);

drvstus_t uart_open(device_t* devp,void* iopack);
drvstus_t uart_close(device_t* devp,void* iopack);
drvstus_t uart_read(device_t* devp,void* iopack);
drvstus_t uart_write(device_t* devp,void* iopack);
drvstus_t uart_lseek(device_t* devp,void* iopack);
drvstus_t uart_ioctrl(device_t* devp,void* iopack);
drvstus_t uart_dev_start(device_t* devp,void* iopack);
drvstus_t uart_dev_stop(device_t* devp,void* iopack);
drvstus_t uart_set_powerstus(device_t* devp,void* iopack);
drvstus_t uart_enum_dev(device_t* devp,void* iopack);
drvstus_t uart_flush(device_t* devp,void* iopack);
drvstus_t uart_shutdown(device_t* devp,void* iopack);

#endif // DRVUART_H
