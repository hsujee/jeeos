/*
 * @Author: Jee Hsu
 * @Description: 随机数设备驱动
 *
 * 实现两个虚拟设备：
 * - random: 伪随机数生成器
 * - urandom: 非阻塞伪随机数生成器
 *
 * 使用线性同余生成器(LCG)算法：
 * - 种子来自系统时钟和TSC
 * - 周期性重新播种增加随机性
 *
 * 驱动接口：
 * - random_read: 读取随机字节
 * - random_write: 向熵池添加随机数据
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "drv_deps.h"

/*===========================================================================
 * 常量定义
 *===========================================================================*/

#define RANDOM_DEVICE_STYPE     0   /* random 设备子类型 */
#define URANDOM_DEVICE_STYPE    1   /* urandom 设备子类型 */

/* LCG参数 (来自Numerical Recipes) */
#define LCG_A   1664525UL
#define LCG_C   1013904223UL
#define LCG_M   0xFFFFFFFFUL

/* 熵池大小 */
#define ENTROPY_POOL_SIZE   256

/*===========================================================================
 * 内部状态
 *===========================================================================*/

PRIVATE u32_t random_state = 0x12345678;    /* LCG状态 */
PRIVATE u8_t entropy_pool[ENTROPY_POOL_SIZE];
PRIVATE uint_t entropy_index = 0;
PRIVATE spinlock_t random_lock;

/*===========================================================================
 * 内部函数
 *===========================================================================*/

/**
 * @brief 初始化随机数生成器
 */
PRIVATE void random_init_seed(void) {
    /* 使用TSC作为初始种子 */
    u64_t tsc = x86_rdtsc();
    random_state = (u32_t)(tsc ^ (tsc >> 32));
    
    /* 初始化熵池 */
    for (uint_t i = 0; i < ENTROPY_POOL_SIZE; i++) {
        entropy_pool[i] = (u8_t)(random_state >> (i % 24));
        random_state = (LCG_A * random_state + LCG_C) & LCG_M;
    }
}

/**
 * @brief 混合熵池
 */
PRIVATE void random_mix_pool(void) {
    for (uint_t i = 0; i < ENTROPY_POOL_SIZE; i++) {
        random_state = (LCG_A * random_state + LCG_C) & LCG_M;
        entropy_pool[i] ^= (u8_t)(random_state >> 16);
        entropy_pool[(i + 13) % ENTROPY_POOL_SIZE] ^= entropy_pool[i];
    }
}

/**
 * @brief 获取一个随机字节
 */
PRIVATE u8_t random_get_byte(void) {
    /* 更新LCG状态 */
    random_state = (LCG_A * random_state + LCG_C) & LCG_M;
    
    /* 从熵池获取并混合 */
    entropy_index = (entropy_index + 1) % ENTROPY_POOL_SIZE;
    u8_t result = entropy_pool[entropy_index] ^ (u8_t)(random_state >> 16);
    
    /* 更新熵池 */
    entropy_pool[entropy_index] = result ^ (u8_t)(random_state >> 8);
    
    return result;
}

/**
 * @brief 添加熵数据
 */
PRIVATE void random_add_entropy(u8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        entropy_index = (entropy_index + 1) % ENTROPY_POOL_SIZE;
        entropy_pool[entropy_index] ^= data[i];
        random_state ^= data[i];
        random_state = (LCG_A * random_state + LCG_C) & LCG_M;
    }
    random_mix_pool();
}

/*===========================================================================
 * 驱动程序接口实现
 *===========================================================================*/

/**
 * @brief 设置驱动程序功能派发函数
 */
void random_set_driver(driver_t *drvp) {
    drvp->drv_dipfun[IOIF_CODE_OPEN] = random_open;
    drvp->drv_dipfun[IOIF_CODE_CLOSE] = random_close;
    drvp->drv_dipfun[IOIF_CODE_READ] = random_read;
    drvp->drv_dipfun[IOIF_CODE_WRITE] = random_write;
    drvp->drv_dipfun[IOIF_CODE_LSEEK] = random_lseek;
    drvp->drv_dipfun[IOIF_CODE_IOCTRL] = random_ioctrl;
    drvp->drv_dipfun[IOIF_CODE_DEV_START] = random_dev_start;
    drvp->drv_dipfun[IOIF_CODE_DEV_STOP] = random_dev_stop;
    drvp->drv_dipfun[IOIF_CODE_SET_POWERSTUS] = random_set_powerstus;
    drvp->drv_dipfun[IOIF_CODE_ENUM_DEV] = random_enum_dev;
    drvp->drv_dipfun[IOIF_CODE_FLUSH] = random_flush;
    drvp->drv_dipfun[IOIF_CODE_SHUTDOWN] = random_shutdown;
    drvp->drv_name = "randomdrv";
    return;
}

/**
 * @brief 设置 random 设备属性
 */
void random_set_device(device_t *devp, driver_t *drvp) {
    devp->dev_flags = DEVFLG_SHARE;
    devp->dev_status = DEVSTS_NORML;
    devp->dev_id.dev_mtype = RANDOM_DEVICE;
    devp->dev_id.dev_stype = RANDOM_DEVICE_STYPE;
    devp->dev_id.dev_nr = 0;
    devp->dev_name = "random";
    return;
}

/**
 * @brief 设置 urandom 设备属性
 */
void urandom_set_device(device_t *devp, driver_t *drvp) {
    devp->dev_flags = DEVFLG_SHARE;
    devp->dev_status = DEVSTS_NORML;
    devp->dev_id.dev_mtype = RANDOM_DEVICE;
    devp->dev_id.dev_stype = URANDOM_DEVICE_STYPE;
    devp->dev_id.dev_nr = 0;
    devp->dev_name = "urandom";
    return;
}

/**
 * @brief 驱动程序入口函数
 */
drvstus_t random_entry(driver_t *drvp, uint_t val, void *p) {
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 初始化锁和随机数生成器 */
    spinlock_init(&random_lock);
    random_init_seed();
    
    random_set_driver(drvp);
    
    /* 创建 random 设备 */
    device_t *random_devp = new_device_dsc();
    if (random_devp == NULL) {
        return DFCERRSTUS;
    }
    
    random_set_device(random_devp, drvp);
    
    if (dev_add_driver(random_devp, drvp) == DFCERRSTUS) {
        del_device_dsc(random_devp);
        return DFCERRSTUS;
    }
    
    if (new_device(random_devp) == DFCERRSTUS) {
        del_device_dsc(random_devp);
        return DFCERRSTUS;
    }
    
    /* 创建 urandom 设备 */
    device_t *urandom_devp = new_device_dsc();
    if (urandom_devp == NULL) {
        return DFCERRSTUS;
    }
    
    urandom_set_device(urandom_devp, drvp);
    
    if (dev_add_driver(urandom_devp, drvp) == DFCERRSTUS) {
        del_device_dsc(urandom_devp);
        return DFCERRSTUS;
    }
    
    if (new_device(urandom_devp) == DFCERRSTUS) {
        del_device_dsc(urandom_devp);
        return DFCERRSTUS;
    }
    
    return DFCOKSTUS;
}

/**
 * @brief 驱动程序退出函数
 */
drvstus_t random_exit(driver_t *drvp, uint_t val, void *p) {
    return DFCOKSTUS;
}

/**
 * @brief 打开设备
 */
drvstus_t random_open(device_t *devp, void *iopack) {
    return dev_inc_devcount(devp);
}

/**
 * @brief 关闭设备
 */
drvstus_t random_close(device_t *devp, void *iopack) {
    return dev_dec_devcount(devp);
}

/**
 * @brief 读取随机数据
 */
drvstus_t random_read(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL || obp->on_len == 0) {
        return DFCERRSTUS;
    }
    
    cpuflg_t flags;
    spinlock_cli(&random_lock, &flags);
    
    u8_t *buf = (u8_t *)obp->on_buf;
    for (size_t i = 0; i < obp->on_len; i++) {
        buf[i] = random_get_byte();
    }
    
    spinunlock_sti(&random_lock, &flags);
    
    return DFCOKSTUS;
}

/**
 * @brief 写入随机数据(添加到熵池)
 */
drvstus_t random_write(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL || obp->on_len == 0) {
        return DFCERRSTUS;
    }
    
    cpuflg_t flags;
    spinlock_cli(&random_lock, &flags);
    
    random_add_entropy((u8_t *)obp->on_buf, obp->on_len);
    
    spinunlock_sti(&random_lock, &flags);
    
    return DFCOKSTUS;
}

/**
 * @brief 调整位置 (不支持)
 */
drvstus_t random_lseek(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

/**
 * @brief 设备控制
 */
drvstus_t random_ioctrl(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 启动设备
 */
drvstus_t random_dev_start(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 停止设备
 */
drvstus_t random_dev_stop(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 设置电源状态
 */
drvstus_t random_set_powerstus(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 枚举设备
 */
drvstus_t random_enum_dev(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 刷新缓存
 */
drvstus_t random_flush(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 关闭设备电源
 */
drvstus_t random_shutdown(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

