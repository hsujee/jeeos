/*
 * @Author: Jee Hsu
 * @Description: 驱动注册辅助函数实现
 *
 * 统一所有驱动的注册流程，减少代码重复。
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "config.h"
#include "btypes.h"
#include "arch_types.h"
#include "kernel_types.h"
#include "systypes.h"
#include "arch_heads.h"
#include "kernel_heads.h"
#include "driver_helper.h"

/*===========================================================================
 * 通用设备注册
 *===========================================================================*/

/**
 * @brief 通用设备注册函数
 */
device_t* register_device_common(
    driver_t *drvp,
    const dev_reg_info_t *info,
    void (*set_driver_fn)(driver_t*)
) {
    if (drvp == NULL || info == NULL || set_driver_fn == NULL) {
        return NULL;
    }
    
    /* 设置驱动功能回调 */
    set_driver_fn(drvp);
    
    /* 创建设备描述符 */
    device_t *devp = new_device_dsc();
    if (devp == NULL) {
        return NULL;
    }
    
    /* 设置设备属性 */
    devp->dev_flags = info->dev_flags;
    devp->dev_status = DEVSTS_NORML;
    devp->dev_id.dev_mtype = info->dev_mtype;
    devp->dev_id.dev_stype = info->dev_stype;
    devp->dev_id.dev_nr = info->dev_nr;
    devp->dev_name = info->dev_name;
    
    /* 绑定设备到驱动 */
    if (dev_add_driver(devp, drvp) == DFCERRSTUS) {
        del_device_dsc(devp);
        return NULL;
    }
    
    /* 注册设备到系统 */
    if (new_device(devp) == DFCERRSTUS) {
        del_device_dsc(devp);
        return NULL;
    }
    
    return devp;
}

/**
 * @brief 注册带中断处理的设备（不自动启用中断）
 */
device_t* register_device_with_irq(
    driver_t *drvp,
    const dev_reg_info_t *info,
    void (*set_driver_fn)(driver_t*),
    int_handler_t irq_handler,
    uint_t irq_vector
) {
    /* 首先注册基本设备 */
    device_t *devp = register_device_common(drvp, info, set_driver_fn);
    if (devp == NULL) {
        return NULL;
    }
    
    /* 注册中断处理函数（但不启用中断） */
    if (new_devhandle(devp, irq_handler, irq_vector) == DFCERRSTUS) {
        /* 失败时清理设备（理想情况下应该有 unregister_device） */
        return NULL;
    }
    
    /* 注意：不在这里启用中断！由调用者在硬件初始化完成后启用 */
    
    return devp;
}

/*===========================================================================
 * 默认设备操作函数
 *===========================================================================*/

/**
 * @brief 默认设备打开函数
 */
drvstus_t device_default_open(device_t *devp, void *iopack) {
    return dev_inc_devcount(devp);
}

/**
 * @brief 默认设备关闭函数
 */
drvstus_t device_default_close(device_t *devp, void *iopack) {
    return dev_dec_devcount(devp);
}

