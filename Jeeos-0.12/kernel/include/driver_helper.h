/*
 * @Author: Jee Hsu
 * @Description: 驱动注册辅助函数
 *
 * 提供统一的驱动注册接口，减少重复代码：
 * - 通用设备注册
 * - 带中断的设备注册
 * - 默认设备操作函数
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _DRIVER_HELPER_H
#define _DRIVER_HELPER_H

/*===========================================================================
 * 设备注册辅助结构
 *===========================================================================*/

/**
 * @brief 驱动设备注册信息结构
 */
typedef struct {
    const char *dev_name;           /* 设备名称 */
    uint_t dev_mtype;               /* 设备主类型 */
    uint_t dev_stype;               /* 设备子类型 */
    uint_t dev_nr;                  /* 设备号 */
    uint_t dev_flags;               /* 设备标志 (DEVFLG_SHARE/EXCLU) */
} dev_reg_info_t;

/*===========================================================================
 * 设备注册函数
 *===========================================================================*/

/**
 * @brief 通用设备注册函数
 * @param drvp 驱动描述符
 * @param info 设备注册信息
 * @param set_driver_fn 设置驱动函数的回调
 * @return 成功返回设备指针，失败返回NULL
 */
device_t* register_device_common(
    driver_t *drvp,
    const dev_reg_info_t *info,
    void (*set_driver_fn)(driver_t*)
);

/**
 * @brief 注册带中断处理的设备
 * @param drvp 驱动描述符
 * @param info 设备注册信息
 * @param set_driver_fn 设置驱动函数的回调
 * @param irq_handler 中断处理函数
 * @param irq_vector 中断向量号
 * @return 成功返回设备指针，失败返回NULL
 */
device_t* register_device_with_irq(
    driver_t *drvp,
    const dev_reg_info_t *info,
    void (*set_driver_fn)(driver_t*),
    int_handler_t irq_handler,
    uint_t irq_vector
);

/*===========================================================================
 * 默认设备操作函数
 *===========================================================================*/

/**
 * @brief 默认设备打开函数（增加引用计数）
 */
drvstus_t device_default_open(device_t *devp, void *iopack);

/**
 * @brief 默认设备关闭函数（减少引用计数）
 */
drvstus_t device_default_close(device_t *devp, void *iopack);

#endif // _DRIVER_HELPER_H

