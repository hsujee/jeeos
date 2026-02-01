/*
 * @Author: Jee Hsu
 * @Description: NULL/ZERO 虚拟设备驱动
 *
 * 实现两个经典的虚拟设备：
 * - null: 丢弃所有写入的数据，读取立即返回(EOF)
 * - zero: 读取返回无限的零字节，丢弃写入
 *
 * 用途：
 * - null: 丢弃不需要的输出
 * - zero: 生成零字节流，用于清零内存/文件
 *
 * 这是最简单的设备驱动示例，适合学习驱动开发基础。
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "drv_deps.h"

/*===========================================================================
 * 设备子类型定义
 *===========================================================================*/

#define NULL_DEVICE_STYPE   0   /* null 设备子类型 */
#define ZERO_DEVICE_STYPE   1   /* zero 设备子类型 */

/*===========================================================================
 * 驱动程序接口实现
 *===========================================================================*/

/**
 * @brief 设置驱动程序功能派发函数
 */
void nullzero_set_driver(driver_t *drvp) {
    drvp->drv_dipfun[IOIF_CODE_OPEN] = nullzero_open;
    drvp->drv_dipfun[IOIF_CODE_CLOSE] = nullzero_close;
    drvp->drv_dipfun[IOIF_CODE_READ] = nullzero_read;
    drvp->drv_dipfun[IOIF_CODE_WRITE] = nullzero_write;
    drvp->drv_dipfun[IOIF_CODE_LSEEK] = nullzero_lseek;
    drvp->drv_dipfun[IOIF_CODE_IOCTRL] = nullzero_ioctrl;
    drvp->drv_dipfun[IOIF_CODE_DEV_START] = nullzero_dev_start;
    drvp->drv_dipfun[IOIF_CODE_DEV_STOP] = nullzero_dev_stop;
    drvp->drv_dipfun[IOIF_CODE_SET_POWERSTUS] = nullzero_set_powerstus;
    drvp->drv_dipfun[IOIF_CODE_ENUM_DEV] = nullzero_enum_dev;
    drvp->drv_dipfun[IOIF_CODE_FLUSH] = nullzero_flush;
    drvp->drv_dipfun[IOIF_CODE_SHUTDOWN] = nullzero_shutdown;
    drvp->drv_name = "nullzerodrv";
    return;
}


drvstus_t nullzero_entry(driver_t *drvp, uint_t val, void *p) {
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 设置驱动功能 */
    nullzero_set_driver(drvp);
    
    /* 创建 null 设备 */
    dev_reg_info_t null_info = {
        .dev_name = "null",
        .dev_mtype = VIR_DEVICE,
        .dev_stype = NULL_DEVICE_STYPE,
        .dev_nr = 0,
        .dev_flags = DEVFLG_SHARE
    };
    
    device_t *null_devp = register_device_common(drvp, &null_info, nullzero_set_driver);
    if (null_devp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 创建 zero 设备 */
    dev_reg_info_t zero_info = {
        .dev_name = "zero",
        .dev_mtype = VIR_DEVICE,
        .dev_stype = ZERO_DEVICE_STYPE,
        .dev_nr = 0,
        .dev_flags = DEVFLG_SHARE
    };
    
    device_t *zero_devp = register_device_common(drvp, &zero_info, nullzero_set_driver);
    if (zero_devp == NULL) {
        return DFCERRSTUS;
    }
    
    return DFCOKSTUS;
}

/**
 * @brief 驱动程序退出函数
 */
drvstus_t nullzero_exit(driver_t *drvp, uint_t val, void *p) {
    return DFCOKSTUS;
}

drvstus_t nullzero_open(device_t *devp, void *iopack) {
    return device_default_open(devp, iopack);
}

drvstus_t nullzero_close(device_t *devp, void *iopack) {
    return device_default_close(devp, iopack);
}

/**
 * @brief 读取设备
 * 
 * - null: 读取返回0字节(EOF)
 * - zero: 读取返回请求长度的全零数据
 */
drvstus_t nullzero_read(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL) {
        return DFCERRSTUS;
    }
    
    /* 根据设备子类型处理 */
    if (devp->dev_id.dev_stype == NULL_DEVICE_STYPE) {
        /* null 设备: 读取返回 0 字节 (EOF) */
        obp->on_len = 0;
    } else if (devp->dev_id.dev_stype == ZERO_DEVICE_STYPE) {
        /* zero 设备: 返回全零数据 */
        arch_memset(obp->on_buf, 0, obp->on_len);
        /* on_len 保持不变，表示成功读取了请求的字节数 */
    } else {
        return DFCERRSTUS;
    }
    
    return DFCOKSTUS;
}

/**
 * @brief 写入设备
 * 
 * - null: 丢弃所有数据，返回成功
 * - zero: 丢弃所有数据，返回成功
 */
drvstus_t nullzero_write(device_t *devp, void *iopack) {
    /* 两个设备都丢弃写入的数据，直接返回成功 */
    return DFCOKSTUS;
}

/**
 * @brief 调整位置 (虚拟设备不支持)
 */
drvstus_t nullzero_lseek(device_t *devp, void *iopack) {
    /* 虚拟设备没有位置概念，但返回成功以保持兼容 */
    return DFCOKSTUS;
}

/**
 * @brief 设备控制
 */
drvstus_t nullzero_ioctrl(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 启动设备
 */
drvstus_t nullzero_dev_start(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 停止设备
 */
drvstus_t nullzero_dev_stop(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 设置电源状态
 */
drvstus_t nullzero_set_powerstus(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 枚举设备
 */
drvstus_t nullzero_enum_dev(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 刷新缓存
 */
drvstus_t nullzero_flush(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 关闭设备电源
 */
drvstus_t nullzero_shutdown(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

