/*
 * @Author: Jee Hsu
 * @Description: RTC 实时时钟驱动
 *
 * 使用CMOS RTC读取/设置系统时间：
 * - 端口0x70: 地址端口
 * - 端口0x71: 数据端口
 *
 * 支持的时间信息：
 * - 秒、分、时 (24小时制)
 * - 日、月、年
 * - 星期
 *
 * 驱动接口：
 * - rtc_read: 读取当前时间
 * - rtc_write: 设置时间
 * - rtc_ioctrl: 获取/设置单独的时间字段
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "drv_deps.h"

/*===========================================================================
 * 端口和寄存器定义
 *===========================================================================*/

#define RTC_ADDR_PORT   0x70    /* RTC地址端口 */
#define RTC_DATA_PORT   0x71    /* RTC数据端口 */

/* CMOS寄存器索引 */
#define RTC_SECONDS     0x00    /* 秒 */
#define RTC_MINUTES     0x02    /* 分 */
#define RTC_HOURS       0x04    /* 时 */
#define RTC_DAY_WEEK    0x06    /* 星期 */
#define RTC_DAY_MONTH   0x07    /* 日 */
#define RTC_MONTH       0x08    /* 月 */
#define RTC_YEAR        0x09    /* 年(后两位) */
#define RTC_CENTURY     0x32    /* 世纪(可能不可用) */
#define RTC_STATUS_A    0x0A    /* 状态寄存器A */
#define RTC_STATUS_B    0x0B    /* 状态寄存器B */
#define RTC_STATUS_C    0x0C    /* 状态寄存器C */

/* 状态寄存器B标志 */
#define RTC_BCD_MODE    0x04    /* BCD模式标志 */
#define RTC_24H_MODE    0x02    /* 24小时模式标志 */

/* ioctl 命令 */
#define RTC_IOCTL_GET_TIME  1   /* 获取时间 */
#define RTC_IOCTL_SET_TIME  2   /* 设置时间 */
#define RTC_IOCTL_GET_ALARM 3   /* 获取闹钟 */
#define RTC_IOCTL_SET_ALARM 4   /* 设置闹钟 */

/*===========================================================================
 * RTC时间结构
 *===========================================================================*/

typedef struct {
    u8_t second;    /* 秒 (0-59) */
    u8_t minute;    /* 分 (0-59) */
    u8_t hour;      /* 时 (0-23) */
    u8_t day;       /* 日 (1-31) */
    u8_t month;     /* 月 (1-12) */
    u16_t year;     /* 年 (完整年份) */
    u8_t weekday;   /* 星期 (1-7, 1=周日) */
} rtc_time_t;

/*===========================================================================
 * 内部函数
 *===========================================================================*/

/**
 * @brief 读取CMOS寄存器
 */
PRIVATE u8_t rtc_read_reg(u8_t reg) {
    out_u8(RTC_ADDR_PORT, reg);
    return in_u8(RTC_DATA_PORT);
}

/**
 * @brief 写入CMOS寄存器
 */
PRIVATE void rtc_write_reg(u8_t reg, u8_t val) {
    out_u8(RTC_ADDR_PORT, reg);
    out_u8(RTC_DATA_PORT, val);
}

/**
 * @brief 检查RTC是否正在更新
 */
PRIVATE bool_t rtc_is_updating(void) {
    return (rtc_read_reg(RTC_STATUS_A) & 0x80) != 0;
}

/**
 * @brief BCD转二进制 (8位版本)
 */
PRIVATE u8_t rtc_bcd_to_bin(u8_t bcd) {
    return (u8_t)((bcd & 0x0F) + ((bcd >> 4) * 10));
}

/**
 * @brief 二进制转BCD (8位版本)
 */
PRIVATE u8_t rtc_bin_to_bcd(u8_t bin) {
    return (u8_t)(((bin / 10) << 4) | (bin % 10));
}

/**
 * @brief 读取RTC时间
 */
PRIVATE void rtc_get_time(rtc_time_t *time) {
    u8_t status_b;
    u8_t second, minute, hour, day, month, year;
    
    /* 等待RTC更新完成 */
    while (rtc_is_updating());
    
    /* 读取原始值 */
    second = rtc_read_reg(RTC_SECONDS);
    minute = rtc_read_reg(RTC_MINUTES);
    hour = rtc_read_reg(RTC_HOURS);
    day = rtc_read_reg(RTC_DAY_MONTH);
    month = rtc_read_reg(RTC_MONTH);
    year = rtc_read_reg(RTC_YEAR);
    time->weekday = rtc_read_reg(RTC_DAY_WEEK);
    
    /* 检查是否为BCD模式 */
    status_b = rtc_read_reg(RTC_STATUS_B);
    
    if (!(status_b & RTC_BCD_MODE)) {
        /* BCD模式, 需要转换 */
        time->second = rtc_bcd_to_bin(second);
        time->minute = rtc_bcd_to_bin(minute);
        time->hour = rtc_bcd_to_bin(hour & 0x7F);  /* 去除AM/PM标志 */
        time->day = rtc_bcd_to_bin(day);
        time->month = rtc_bcd_to_bin(month);
        year = rtc_bcd_to_bin(year);
    } else {
        time->second = second;
        time->minute = minute;
        time->hour = hour & 0x7F;
        time->day = day;
        time->month = month;
    }
    
    /* 处理12/24小时制 */
    if (!(status_b & RTC_24H_MODE) && (hour & 0x80)) {
        time->hour = (time->hour % 12) + 12;  /* PM */
    }
    
    /* 计算完整年份 (假设2000年代) */
    time->year = 2000 + year;
    if (year > 99) {
        time->year = 1900 + year;
    }
}

/**
 * @brief 设置RTC时间
 */
PRIVATE void rtc_set_time(rtc_time_t *time) {
    u8_t status_b;
    
    /* 等待RTC更新完成 */
    while (rtc_is_updating());
    
    status_b = rtc_read_reg(RTC_STATUS_B);
    
    u8_t year = (u8_t)(time->year % 100);
    u8_t second = time->second;
    u8_t minute = time->minute;
    u8_t hour = time->hour;
    u8_t day = time->day;
    u8_t month = time->month;
    
    if (!(status_b & RTC_BCD_MODE)) {
        /* 转换为BCD */
        second = rtc_bin_to_bcd(second);
        minute = rtc_bin_to_bcd(minute);
        hour = rtc_bin_to_bcd(hour);
        day = rtc_bin_to_bcd(day);
        month = rtc_bin_to_bcd(month);
        year = rtc_bin_to_bcd(year);
    }
    
    rtc_write_reg(RTC_SECONDS, second);
    rtc_write_reg(RTC_MINUTES, minute);
    rtc_write_reg(RTC_HOURS, hour);
    rtc_write_reg(RTC_DAY_MONTH, day);
    rtc_write_reg(RTC_MONTH, month);
    rtc_write_reg(RTC_YEAR, year);
}

/*===========================================================================
 * 驱动程序接口实现
 *===========================================================================*/

/**
 * @brief 设置驱动程序功能派发函数
 */
void rtc_set_driver(driver_t *drvp) {
    drvp->drv_dipfun[IOIF_CODE_OPEN] = rtc_open;
    drvp->drv_dipfun[IOIF_CODE_CLOSE] = rtc_close;
    drvp->drv_dipfun[IOIF_CODE_READ] = rtc_read;
    drvp->drv_dipfun[IOIF_CODE_WRITE] = rtc_write;
    drvp->drv_dipfun[IOIF_CODE_LSEEK] = rtc_lseek;
    drvp->drv_dipfun[IOIF_CODE_IOCTRL] = rtc_ioctrl;
    drvp->drv_dipfun[IOIF_CODE_DEV_START] = rtc_dev_start;
    drvp->drv_dipfun[IOIF_CODE_DEV_STOP] = rtc_dev_stop;
    drvp->drv_dipfun[IOIF_CODE_SET_POWERSTUS] = rtc_set_powerstus;
    drvp->drv_dipfun[IOIF_CODE_ENUM_DEV] = rtc_enum_dev;
    drvp->drv_dipfun[IOIF_CODE_FLUSH] = rtc_flush;
    drvp->drv_dipfun[IOIF_CODE_SHUTDOWN] = rtc_shutdown;
    drvp->drv_name = "rtcdrv";
    return;
}

/**
 * @brief 设置设备属性
 */
void rtc_set_device(device_t *devp, driver_t *drvp) {
    devp->dev_flags = DEVFLG_SHARE;     /* 可共享访问 */
    devp->dev_status = DEVSTS_NORML;
    devp->dev_id.dev_mtype = RTC_DEVICE;
    devp->dev_id.dev_stype = 0;
    devp->dev_id.dev_nr = 0;
    devp->dev_name = "rtc";
    return;
}

/**
 * @brief 驱动程序入口函数
 */
drvstus_t rtc_entry(driver_t *drvp, uint_t val, void *p) {
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    
    device_t *devp = new_device_dsc();
    if (devp == NULL) {
        return DFCERRSTUS;
    }
    
    rtc_set_driver(drvp);
    rtc_set_device(devp, drvp);
    
    if (dev_add_driver(devp, drvp) == DFCERRSTUS) {
        del_device_dsc(devp);
        return DFCERRSTUS;
    }
    
    if (new_device(devp) == DFCERRSTUS) {
        del_device_dsc(devp);
        return DFCERRSTUS;
    }
    
    return DFCOKSTUS;
}

/**
 * @brief 驱动程序退出函数
 */
drvstus_t rtc_exit(driver_t *drvp, uint_t val, void *p) {
    return DFCOKSTUS;
}

/**
 * @brief 打开设备
 */
drvstus_t rtc_open(device_t *devp, void *iopack) {
    return dev_inc_devcount(devp);
}

/**
 * @brief 关闭设备
 */
drvstus_t rtc_close(device_t *devp, void *iopack) {
    return dev_dec_devcount(devp);
}

/**
 * @brief 读取时间
 * 
 * 读取rtc_time_t结构
 */
drvstus_t rtc_read(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL || obp->on_len < sizeof(rtc_time_t)) {
        return DFCERRSTUS;
    }
    
    rtc_get_time((rtc_time_t *)obp->on_buf);
    obp->on_len = sizeof(rtc_time_t);
    
    return DFCOKSTUS;
}

/**
 * @brief 写入时间
 * 
 * 写入rtc_time_t结构
 */
drvstus_t rtc_write(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL || obp->on_len < sizeof(rtc_time_t)) {
        return DFCERRSTUS;
    }
    
    rtc_set_time((rtc_time_t *)obp->on_buf);
    
    return DFCOKSTUS;
}

/**
 * @brief 调整位置 (不支持)
 */
drvstus_t rtc_lseek(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

/**
 * @brief 设备控制
 */
drvstus_t rtc_ioctrl(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    switch (obp->on_ioctrd) {
    case RTC_IOCTL_GET_TIME:
        if (obp->on_buf && obp->on_len >= sizeof(rtc_time_t)) {
            rtc_get_time((rtc_time_t *)obp->on_buf);
            return DFCOKSTUS;
        }
        break;
    case RTC_IOCTL_SET_TIME:
        if (obp->on_buf && obp->on_len >= sizeof(rtc_time_t)) {
            rtc_set_time((rtc_time_t *)obp->on_buf);
            return DFCOKSTUS;
        }
        break;
    default:
        break;
    }
    
    return DFCERRSTUS;
}

/**
 * @brief 启动设备
 */
drvstus_t rtc_dev_start(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 停止设备
 */
drvstus_t rtc_dev_stop(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 设置电源状态
 */
drvstus_t rtc_set_powerstus(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 枚举设备
 */
drvstus_t rtc_enum_dev(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 刷新缓存
 */
drvstus_t rtc_flush(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 关闭设备电源
 */
drvstus_t rtc_shutdown(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

