/*
 * @Author: Jee Hsu
 * @Description: 时间管理子系统
 *
 * 管理系统时间和定时器：
 * - 系统时间维护(年/月/日/时/分/秒)
 * - 系统滴答计数(systick)
 * - 定时器支持
 *
 * 核心数据结构：
 * - ktime_t: 内核时间结构
 * - systick: 系统启动后的滴答数
 *
 * 时间来源：
 * - RTC(实时时钟): 提供日期时间
 * - PIT/HPET: 提供定时中断
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

void ktime_t_init(ktime_t *initp) {
    spinlock_init(&initp->kt_lock);
    initp->kt_year = 0;
    initp->kt_mon = 0;
    initp->kt_day = 0;
    initp->kt_date = 0;
    initp->kt_hour = 0;
    initp->kt_min = 0;
    initp->kt_sec = 0;
    initp->kt_systick = 0;
    initp->kt_datap = NULL;
    return;
}

void init_ktime() {
    ktime_t_init(&osktime);
    /* 时间子系统初始化完成 */
    return;
}

void update_times_from_jeeos() {
    ktime_t *initp = &osktime;
    u8_t tmptm;
    cpuflg_t cpufg;
    spinlock_cli(&initp->kt_lock, &cpufg);
	JEEOS_READ(tmptm,JEEOS_SEC_ADR);
    initp->kt_sec = bcd_to_bin(tmptm);
    JEEOS_READ(tmptm,JEEOS_MIN_ADR);
    initp->kt_min = bcd_to_bin(tmptm);
    JEEOS_READ(tmptm,JEEOS_HOUR_ADR);
    initp->kt_hour = bcd_to_bin(tmptm);
    JEEOS_READ(tmptm,JEEOS_DAY_ADR);
    initp->kt_day = bcd_to_bin(tmptm);
    JEEOS_READ(tmptm,JEEOS_MON_ADR);
    initp->kt_mon = bcd_to_bin(tmptm);
    JEEOS_READ(tmptm,JEEOS_YEAR_ADR);
    initp->kt_year = bcd_to_bin(tmptm);
    // printk("osktime y:%d,m:%x,d:%d,h:%d,m:%d,s:%d\n", initp->kt_year, initp->kt_mon,initp->kt_day,
    //                                                     initp->kt_hour, initp->kt_min, initp->kt_sec);
    spinunlock_sti(&initp->kt_lock, &cpufg);
    return;
}

void update_times(uint_t year, uint_t mon, uint_t day, uint_t date, uint_t hour, uint_t min, uint_t sec) {
    ktime_t *initp = &osktime;
    cpuflg_t cpufg;
    spinlock_cli(&initp->kt_lock, &cpufg);
    initp->kt_year = year;
    initp->kt_mon = mon;
    initp->kt_day = day;
    initp->kt_date = date;
    initp->kt_hour = hour;
    initp->kt_min = min;
    initp->kt_sec = sec;
    spinunlock_sti(&initp->kt_lock, &cpufg);
    return;
}

void inc_systick() {
    osktime.kt_systick++;
    return;
}

u64_t get_systick() {
    return osktime.kt_systick;
}
