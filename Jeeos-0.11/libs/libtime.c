/*
 * @Author: Jee Hsu
 * @Description: 用户态库 - 时间管理
 *
 * 提供用户态的时间操作函数：
 * - time(): 获取系统时间（通过系统调用）
 * - settime(): 设置RTC时间
 * - gettime(): 获取RTC时间
 * - synsecalarm(): 设置秒级闹钟
 *
 * 底层实现：
 * - time() 通过系统调用直接获取
 * - 其他函数通过打开RTC设备进行操作
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "libtypes.h"
#include "libsyscalls.h"
#include "libio.h"
#include "libdev.h"

/* RTC设备IOCTL控制码 */
#define IOCTRCODE_SETTIME       1
#define IOCTRCODE_SET_SYNSECALM 2

/**
 * @brief 获取系统时间（通过系统调用）
 * @param ttime 时间结构体指针
 * @return 成功返回SYSSTUSOK，失败返回SYSSTUSERR
 */
sysstus_t time(times_t* ttime) {
    sysstus_t rets;
    __SYSCALL_PARAM1(INR_TIME, rets, ttime);
    return rets;
}

/**
 * @brief 设置RTC时间
 * @param ttime 要设置的时间
 * @return 成功返回SYSSTUSOK，失败返回SYSSTUSERR
 */
sysstus_t settime(times_t* ttime) {
    if (ttime == NULL) {
        return SYSSTUSERR;
    }
    devid_t dev;
    dev.dev_mtype = RTC_DEVICE;
    dev.dev_stype = 0;
    dev.dev_nr = 0;
    hand_t fd = open(&dev, RW_FLG | FILE_TY_DEV, 0);
    if (fd == -1) {
        return SYSSTUSERR;
    }
    ioctrl(fd, ttime, IOCTRCODE_SETTIME, 0);
    close(fd);
    return SYSSTUSOK;
}

/**
 * @brief 获取RTC时间
 * @param ttime 存储时间的结构体
 * @return 成功返回SYSSTUSOK，失败返回SYSSTUSERR
 */
sysstus_t gettime(times_t* ttime) {
    if (ttime == NULL) {
        return SYSSTUSERR;
    }
    devid_t dev;
    dev.dev_mtype = RTC_DEVICE;
    dev.dev_stype = 0;
    dev.dev_nr = 0;
    hand_t fd = open(&dev, RW_FLG | FILE_TY_DEV, 0);
    if (fd == -1) {
        return SYSSTUSERR;
    }
    read(fd, ttime, sizeof(times_t), 0);
    close(fd);
    return SYSSTUSOK;
}

/**
 * @brief 设置秒级同步闹钟
 * @param sec 秒数（0-59）
 * @return 成功返回SYSSTUSOK，失败返回SYSSTUSERR
 */
sysstus_t synsecalarm(uint_t sec) {
    uint_t tmpsec = sec;
    if (tmpsec >= 60) {
        return SYSSTUSERR;
    }
    devid_t dev;
    dev.dev_mtype = RTC_DEVICE;
    dev.dev_stype = 0;
    dev.dev_nr = 0;
    hand_t fd = open(&dev, RW_FLG | FILE_TY_DEV, 0);
    if (fd == -1) {
        return SYSSTUSERR;
    }
    ioctrl(fd, &tmpsec, IOCTRCODE_SET_SYNSECALM, 0);
    close(fd);
    return SYSSTUSOK;
}

/*===========================================================================
 * 延时函数
 *===========================================================================*/

/* 简单延时循环计数 (需要根据实际CPU频率调整) */
#define DELAY_LOOP_COUNT    1000000UL

/**
 * @brief 秒级睡眠
 * @param seconds 睡眠秒数
 * @return 成功返回0，被中断返回剩余秒数
 * @note 当前为简化实现，未来可改进为真正的调度睡眠
 */
uint_t sleep(uint_t seconds) {
    volatile uint_t i, j;
    
    for (i = 0; i < seconds; i++) {
        /* 每秒约循环 DELAY_LOOP_COUNT 次 */
        for (j = 0; j < DELAY_LOOP_COUNT; j++) {
            __asm__ __volatile__("pause" ::: "memory");
        }
    }
    
    return 0;
}

/**
 * @brief 微秒级睡眠
 * @param usec 睡眠微秒数
 * @return 成功返回0
 * @note 当前为简化实现，精度有限
 */
int usleep(uint_t usec) {
    volatile uint_t i;
    uint_t loops = (usec * DELAY_LOOP_COUNT) / 1000000UL;
    
    if (loops == 0 && usec > 0) {
        loops = 1;
    }
    
    for (i = 0; i < loops; i++) {
        __asm__ __volatile__("pause" ::: "memory");
    }
    
    return 0;
}

/**
 * @brief 毫秒级睡眠
 * @param msec 睡眠毫秒数
 * @return 成功返回0
 */
int msleep(uint_t msec) {
    return usleep(msec * 1000);
}
