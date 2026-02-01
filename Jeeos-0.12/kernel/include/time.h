/*
 * @Author: Jee Hsu
 * @Description: 时间管理 - 系统时间和定时器
 *
 * 提供系统时间管理功能：
 * - RTC时间读取
 * - 系统时间维护
 * - BCD码转换
 * - 时间系统调用
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _TIME_H
#define _TIME_H

/**
 * @brief 初始化时间结构
 * @param initp 时间结构指针
 */
void ktime_t_init(ktime_t* initp);

/**
 * @brief 初始化时间子系统
 * 
 * 从RTC读取当前时间并初始化系统时钟
 */
void init_ktime();

/**
 * @brief 从系统更新时间
 */
void update_times_from_jeeos();

/**
 * @brief 增加系统滴答计数
 */
void inc_systick();

/**
 * @brief 获取系统滴答计数
 * @return 系统启动后的滴答数
 */
u64_t get_systick();

/**
 * @brief 更新系统时间
 * @param year 年
 * @param mon 月
 * @param day 日
 * @param date 星期
 * @param hour 时
 * @param min 分
 * @param sec 秒
 */
void update_times(uint_t year, uint_t mon, uint_t day, uint_t date, uint_t hour, uint_t min, uint_t sec);

/**
 * @brief 系统调用表time入口
 * @param inr 系统调用号
 * @param stkparv 栈参数
 * @return 系统调用状态码
 */
sysstus_t syscall_time(uint_t inr, stkparame_t *stkparv);

/**
 * @brief 获取系统时间
 * @param time 时间结构指针
 * @return 系统调用状态码
 */
sysstus_t sys_time(time_t *time);

/**
 * @brief BCD码转二进制
 * @param val BCD编码值
 * @return 二进制值
 */
KLINE u32_t bcd_to_bin(u32_t val) {
	return (val & 0xf) + ((val >> 4) * 10);
}

#endif // _TIME_H
