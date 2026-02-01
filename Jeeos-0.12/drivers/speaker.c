/*
 * @Author: Jee Hsu
 * @Description: PC Speaker 蜂鸣器驱动
 *
 * 使用8254定时器Channel 2控制PC蜂鸣器：
 * - 通过端口0x42设置频率
 * - 通过端口0x61控制开关
 *
 * 驱动接口：
 * - speaker_open/close: 打开/关闭设备
 * - speaker_write: 写入频率值(Hz)，0表示静音
 * - speaker_ioctrl: 控制蜂鸣器开关
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "drv_deps.h"

/*===========================================================================
 * 端口和常量定义
 *===========================================================================*/

#define SPEAKER_PORT        0x61    /* 蜂鸣器控制端口 */
#define TIMER_CH2_PORT      0x42    /* 定时器通道2数据端口 */
#define TIMER_MODE_PORT     0x43    /* 定时器模式控制端口 */

#define TIMER_CH2_MODE      0xB6    /* 通道2: 方波模式, 先低后高 */
#define PIT_FREQUENCY       1193182 /* 8254输入时钟频率 */

#define SPEAKER_ENABLE_MASK 0x03    /* 启用蜂鸣器的位掩码 */

/* ioctl 命令 */
#define SPEAKER_IOCTL_ON    1       /* 开启蜂鸣器 */
#define SPEAKER_IOCTL_OFF   2       /* 关闭蜂鸣器 */
#define SPEAKER_IOCTL_BEEP  3       /* 蜂鸣(频率+时长) */

/*===========================================================================
 * 内部函数
 *===========================================================================*/

/**
 * @brief 设置蜂鸣器频率
 * @param freq 频率(Hz), 0表示静音
 */
PRIVATE void speaker_set_freq(u32_t freq) {
    if (freq == 0) {
        /* 静音: 关闭蜂鸣器 */
        u8_t val = in_u8(SPEAKER_PORT);
        out_u8(SPEAKER_PORT, val & ~SPEAKER_ENABLE_MASK);
        return;
    }
    
    /* 计算分频值 */
    u32_t divisor = PIT_FREQUENCY / freq;
    if (divisor > 0xFFFF) divisor = 0xFFFF;
    if (divisor < 1) divisor = 1;
    
    /* 设置定时器通道2 */
    out_u8(TIMER_MODE_PORT, TIMER_CH2_MODE);
    out_u8(TIMER_CH2_PORT, (u8_t)(divisor & 0xFF));
    out_u8(TIMER_CH2_PORT, (u8_t)((divisor >> 8) & 0xFF));
    
    /* 开启蜂鸣器 */
    u8_t val = in_u8(SPEAKER_PORT);
    out_u8(SPEAKER_PORT, val | SPEAKER_ENABLE_MASK);
}

/**
 * @brief 关闭蜂鸣器
 */
PRIVATE void speaker_off(void) {
    u8_t val = in_u8(SPEAKER_PORT);
    out_u8(SPEAKER_PORT, val & ~SPEAKER_ENABLE_MASK);
}

/**
 * @brief 开启蜂鸣器
 */
PRIVATE void speaker_on(void) {
    u8_t val = in_u8(SPEAKER_PORT);
    out_u8(SPEAKER_PORT, val | SPEAKER_ENABLE_MASK);
}

/*===========================================================================
 * 驱动程序接口实现
 *===========================================================================*/

/**
 * @brief 设置驱动程序功能派发函数
 */
void speaker_set_driver(driver_t *drvp) {
    drvp->drv_dipfun[IOIF_CODE_OPEN] = speaker_open;
    drvp->drv_dipfun[IOIF_CODE_CLOSE] = speaker_close;
    drvp->drv_dipfun[IOIF_CODE_READ] = speaker_read;
    drvp->drv_dipfun[IOIF_CODE_WRITE] = speaker_write;
    drvp->drv_dipfun[IOIF_CODE_LSEEK] = speaker_lseek;
    drvp->drv_dipfun[IOIF_CODE_IOCTRL] = speaker_ioctrl;
    drvp->drv_dipfun[IOIF_CODE_DEV_START] = speaker_dev_start;
    drvp->drv_dipfun[IOIF_CODE_DEV_STOP] = speaker_dev_stop;
    drvp->drv_dipfun[IOIF_CODE_SET_POWERSTUS] = speaker_set_powerstus;
    drvp->drv_dipfun[IOIF_CODE_ENUM_DEV] = speaker_enum_dev;
    drvp->drv_dipfun[IOIF_CODE_FLUSH] = speaker_flush;
    drvp->drv_dipfun[IOIF_CODE_SHUTDOWN] = speaker_shutdown;
    drvp->drv_name = "speakerdrv";
    return;
}


drvstus_t speaker_entry(driver_t *drvp, uint_t val, void *p) {
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 注册设备 */
    dev_reg_info_t info = {
        .dev_name = "speaker",
        .dev_mtype = SPEAKER_DEVICE,
        .dev_stype = 0,
        .dev_nr = 0,
        .dev_flags = DEVFLG_EXCLU
    };
    
    device_t *devp = register_device_common(drvp, &info, speaker_set_driver);
    if (devp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 初始化: 确保蜂鸣器关闭 */
    speaker_off();
    
    return DFCOKSTUS;
}

/**
 * @brief 驱动程序退出函数
 */
drvstus_t speaker_exit(driver_t *drvp, uint_t val, void *p) {
    speaker_off();
    return DFCOKSTUS;
}

/**
 * @brief 打开设备
 */
drvstus_t speaker_open(device_t *devp, void *iopack) {
    return dev_inc_devcount(devp);
}

/**
 * @brief 关闭设备
 */
drvstus_t speaker_close(device_t *devp, void *iopack) {
    speaker_off();  /* 关闭时静音 */
    return dev_dec_devcount(devp);
}

/**
 * @brief 读取设备 (不支持)
 */
drvstus_t speaker_read(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

/**
 * @brief 写入设备 - 设置频率
 * 
 * 写入一个32位无符号整数表示频率(Hz)
 * 0 表示静音
 */
drvstus_t speaker_write(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL || obp->on_len < sizeof(u32_t)) {
        return DFCERRSTUS;
    }
    
    u32_t freq = *(u32_t *)obp->on_buf;
    speaker_set_freq(freq);
    
    return DFCOKSTUS;
}

/**
 * @brief 调整位置 (不支持)
 */
drvstus_t speaker_lseek(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

/**
 * @brief 设备控制
 * 
 * 命令:
 * - SPEAKER_IOCTL_ON: 开启蜂鸣器
 * - SPEAKER_IOCTL_OFF: 关闭蜂鸣器
 */
drvstus_t speaker_ioctrl(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    switch (obp->on_ioctrd) {
    case SPEAKER_IOCTL_ON:
        speaker_on();
        break;
    case SPEAKER_IOCTL_OFF:
        speaker_off();
        break;
    default:
        return DFCERRSTUS;
    }
    
    return DFCOKSTUS;
}

/**
 * @brief 启动设备
 */
drvstus_t speaker_dev_start(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 停止设备
 */
drvstus_t speaker_dev_stop(device_t *devp, void *iopack) {
    speaker_off();
    return DFCOKSTUS;
}

/**
 * @brief 设置电源状态
 */
drvstus_t speaker_set_powerstus(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 枚举设备
 */
drvstus_t speaker_enum_dev(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 刷新缓存
 */
drvstus_t speaker_flush(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 关闭设备电源
 */
drvstus_t speaker_shutdown(device_t *devp, void *iopack) {
    speaker_off();
    return DFCOKSTUS;
}

