/*
 * @Author: Jee Hsu
 * @Description: 时间管理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _TIME_T_H
#define _TIME_T_H

typedef struct s_KTIME {
    spinlock_t  kt_lock;
    uint_t      kt_year;
    uint_t      kt_mon;
    uint_t      kt_day;
    uint_t      kt_date;
    uint_t      kt_hour;
    uint_t      kt_min;
    uint_t      kt_sec;
    u64_t       kt_systick;     /**< 系统启动后的滴答计数 */
    void*       kt_datap;  
} ktime_t;

typedef struct s_TIME {
    uint_t      year;
    uint_t      mon;
    uint_t      day;
    uint_t      date;
    uint_t      hour;
    uint_t      min;
    uint_t      sec;
} time_t;

#define JEEOS_PROT_ADR 0x70
#define JEEOS_PROT_DATE 0x71
#define JEEOS_SEC_ADR 0x00
#define JEEOS_MIN_ADR 0x02
#define JEEOS_HOUR_ADR 0x04
#define JEEOS_DAY_ADR 0x07
#define JEEOS_MON_ADR 0x08
#define JEEOS_YEAR_ADR 0x09
#define JEEOS_READ(val,adr) ({out_u8(JEEOS_PROT_ADR,adr);val=in_u8(JEEOS_PROT_DATE);}) 

#endif // TIME_T_H
