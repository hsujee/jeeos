/*
 * @Author: Jee Hsu
 * @Description: 系统时钟驱动(8254 PIT)
 *
 * 管理系统时钟中断和计时：
 * - 初始化8254可编程间隔定时器
 * - 处理时钟中断(IRQ0)
 * - 维护系统滴答计数(systick)
 * - 触发进程调度
 *
 * 时钟频率：
 * - 默认100Hz(10ms间隔)
 * - 每次中断systick++
 *
 * 驱动接口：
 * - systick_open/close: 打开/关闭
 * - systick_read: 读取当前systick
 * - systick_handle: 中断处理函数
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "drv_deps.h"

void init_8254() {
    out_u8_p(PTIPROTM, TIMEMODE);
    out_u8_p(PTIPROT1, HZLL);
    out_u8_p(PTIPROT1, HZHH);
    return;
}

void systick_set_driver(driver_t *drvp) {
    //配置驱动程序功能派发函数
    drvp->drv_dipfun[IOIF_CODE_OPEN] = systick_open;
    drvp->drv_dipfun[IOIF_CODE_CLOSE] = systick_close;
    drvp->drv_dipfun[IOIF_CODE_READ] = systick_read;
    drvp->drv_dipfun[IOIF_CODE_WRITE] = systick_write;
    drvp->drv_dipfun[IOIF_CODE_LSEEK] = systick_lseek;
    drvp->drv_dipfun[IOIF_CODE_IOCTRL] = systick_ioctrl;
    drvp->drv_dipfun[IOIF_CODE_DEV_START] = systick_dev_start;
    drvp->drv_dipfun[IOIF_CODE_DEV_STOP] = systick_dev_stop;
    drvp->drv_dipfun[IOIF_CODE_SET_POWERSTUS] = systick_set_powerstus;
    drvp->drv_dipfun[IOIF_CODE_ENUM_DEV] = systick_enum_dev;
    drvp->drv_dipfun[IOIF_CODE_FLUSH] = systick_flush;
    drvp->drv_dipfun[IOIF_CODE_SHUTDOWN] = systick_shutdown;
    //设置驱动程序名称
    drvp->drv_name = "systick0drv";
    return;
}

void systick_set_device(device_t *devp, driver_t *drvp) {
    //设备可共享访问
    devp->dev_flags = DEVFLG_SHARE;
    //设备正常状态
    devp->dev_status = DEVSTS_NORML;
    //设备主类型
    devp->dev_id.dev_mtype = SYSTICK_DEVICE;
    //设备子类型
    devp->dev_id.dev_stype = 0;
    //设备号
    devp->dev_id.dev_nr = 0;
    //设置设备名称
    devp->dev_name = "systick0";
    return;
}

//系统定时驱动程序入口函数
drvstus_t systick_entry(driver_t *drvp, uint_t val, void *p) {
    //drvp是内核传递进来的参数，不能为NULL
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    //建立设备描述符结构的变量实例
    device_t *devp = new_device_dsc();
    //不能失败
    if (devp == NULL) {
        return DFCERRSTUS;
    }
    //驱动程序的功能函数设置到driver_t结构中的drv_dipfun数组中
    systick_set_driver(drvp);
    //将设备挂载到驱动中
    systick_set_device(devp, drvp);
    if (dev_add_driver(devp, drvp) == DFCERRSTUS) {
        //注意释放资源
        if (del_device_dsc(devp) == DFCERRSTUS) {
            return DFCERRSTUS;
        }
        return DFCERRSTUS;
    }
    //向内核注册设备
    if (new_device(devp) == DFCERRSTUS) {
        if (del_device_dsc(devp) == DFCERRSTUS) {
            return DFCERRSTUS;
        }
        return DFCERRSTUS;
    }
    //安装中断回调函数systick_handle
    if (new_devhandle(devp, systick_handle, 0x20) == DFCERRSTUS) {
        return DFCERRSTUS; //注意释放资源。
    }
    //初始化物理设备
    init_8254();

    if (enable_intline(20) == DFCERRSTUS) {
        return DFCERRSTUS;
    }
    return DFCOKSTUS;
}

//系统定时驱动程序退出函数
drvstus_t systick_exit(driver_t *drvp, uint_t val, void *p) {
    return DFCERRSTUS;
}

//系统定时驱动程序中断处理函数
drvstus_t systick_handle(uint_t ift_nr, void *devp, void *sframe) {
    (void)ift_nr;
    (void)devp;
    (void)sframe;
    
    //增加系统滴答计数
    inc_systick();
    
    //更新当前进程的tick (内部会检查时间片并设置调度标志)
    thread_inc_tick(sched_retn_currthread());
    
    //更新系统时间
    update_times_from_jeeos();
    
    return DFCOKSTUS;
}

//打开设备函数
drvstus_t systick_open(device_t *devp, void *iopack) {
    //增加设备计数
    dev_inc_devcount(devp);
    //返回成功完成的状态
    return DFCOKSTUS;
}

//关闭设备函数
drvstus_t systick_close(device_t *devp, void *iopack) {
    //减少设备计数
    dev_dec_devcount(devp);
    //返回成功完成的状态
    return DFCOKSTUS;
}

//读设备数据函数 - 返回系统滴答计数
drvstus_t systick_read(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL || obp->on_bufsz < sizeof(u64_t)) {
        return DFCERRSTUS;
    }
    
    //返回系统启动后的滴答计数
    *(u64_t *)obp->on_buf = get_systick();
    obp->on_len = sizeof(u64_t);
    
    return DFCOKSTUS;
}

//写设备数据函数
drvstus_t systick_write(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//调整读写设备数据位置函数
drvstus_t systick_lseek(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//控制设备函数
drvstus_t systick_ioctrl(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//开启设备函数
drvstus_t systick_dev_start(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//停止设备函数
drvstus_t systick_dev_stop(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//设置设备电源函数
drvstus_t systick_set_powerstus(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//枚举设备函数
drvstus_t systick_enum_dev(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//刷新设备缓存函数
drvstus_t systick_flush(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//设备关机函数
drvstus_t systick_shutdown(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}
