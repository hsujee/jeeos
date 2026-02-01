/*
 * @Author: Jee Hsu
 * @Description: 设备管理模块 - 设备驱动框架
 *
 * 提供统一的设备驱动管理框架：
 * - 设备注册和发现
 * - 驱动程序管理
 * - I/O请求处理
 * - 设备句柄管理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _DEVICE_H
#define _DEVICE_H

/*===========================================================================
 * 初始化函数
 *===========================================================================*/

/**
 * @brief 初始化设备类型链表
 * @param initp 链表指针
 * @param dtype 设备类型
 */
void devtlst_t_init(devtlst_t *initp, uint_t dtype);

/**
 * @brief 初始化设备表
 * @param initp 设备表指针
 */
void devtable_t_init(devtable_t *initp);

/**
 * @brief 初始化设备ID
 * @param initp 设备ID指针
 * @param mty 主类型
 * @param sty 子类型
 * @param nr 设备号
 */
void devid_t_init(devid_t *initp, uint_t mty, uint_t sty, uint_t nr);

/**
 * @brief 初始化设备结构
 * @param initp 设备指针
 */
void device_t_init(device_t *initp);

/**
 * @brief 获取驱动ID
 * @param dverp 驱动指针
 */
void retn_driverid(driver_t *dverp);

/**
 * @brief 初始化驱动结构
 * @param initp 驱动指针
 */
void driver_t_init(driver_t *initp);

/**
 * @brief 初始化设备子系统
 */
void init_device();

/**
 * @brief 运行驱动入口函数
 * @param drventry 驱动入口函数
 * @return 驱动状态码
 */
drvstus_t run_driverentry(drventyexit_t drventry);

/**
 * @brief 初始化所有驱动
 */
void init_driver();

/*===========================================================================
 * 驱动管理函数
 *===========================================================================*/

/**
 * @brief 删除驱动描述符
 * @param drvp 驱动指针
 * @return 驱动状态码
 */
drvstus_t del_driver_dsc(driver_t *drvp);

/**
 * @brief 分配新的驱动描述符
 * @return 驱动指针，失败返回NULL
 */
driver_t *new_driver_dsc();

/**
 * @brief 删除设备描述符
 * @param devp 设备指针
 * @return 驱动状态码
 */
drvstus_t del_device_dsc(device_t *devp);

/**
 * @brief 分配新的设备描述符
 * @return 设备指针，失败返回NULL
 */
device_t *new_device_dsc();

/**
 * @brief 默认驱动函数
 * @param devp 设备指针
 * @param iopack I/O包
 * @return 驱动状态码
 */
drvstus_t drv_defalt_func(device_t *devp, void *iopack);

/**
 * @brief 比较设备ID
 * @param sdidp 源设备ID
 * @param cdidp 比较设备ID
 * @return TRUE匹配，FALSE不匹配
 */
bool_t cmp_devid(devid_t *sdidp, devid_t *cdidp);

/**
 * @brief 将驱动添加到系统
 * @param drvp 驱动指针
 * @return 驱动状态码
 */
drvstus_t driver_add_system(driver_t *drvp);

/**
 * @brief 将设备绑定到驱动
 * @param devp 设备指针
 * @param drvp 驱动指针
 * @return 驱动状态码
 */
drvstus_t dev_add_driver(device_t *devp, driver_t *drvp);

/**
 * @brief 注册新设备
 * @param devp 设备指针
 * @return 驱动状态码
 */
drvstus_t new_device(device_t *devp);

/*===========================================================================
 * 设备操作函数
 *===========================================================================*/

/**
 * @brief 增加设备引用计数
 */
drvstus_t dev_inc_devcount(device_t *devp);

/**
 * @brief 减少设备引用计数
 */
drvstus_t dev_dec_devcount(device_t *devp);

/**
 * @brief 添加I/O请求到设备
 */
drvstus_t dev_add_request(device_t *devp, objnode_t *request);

/**
 * @brief 完成I/O请求
 */
drvstus_t dev_complete_request(device_t *devp, objnode_t *request);

/**
 * @brief 获取设备的I/O请求
 */
drvstus_t dev_retn_request(device_t *devp, uint_t iocode, objnode_t **retreq);

/**
 * @brief 等待I/O请求完成
 */
drvstus_t dev_wait_request(device_t *devp, objnode_t *request);

/**
 * @brief 获取请求参数
 */
drvstus_t dev_retn_rqueparm(void *request, buf_t *retbuf, uint_t *retcops, uint_t *retlen, uint_t *retioclde, uint_t *retbufcops, size_t *retbufsz);

/**
 * @brief 查找设备
 * @param dfname 设备名称
 * @param flgs 查找标志
 * @return 设备指针，未找到返回NULL
 */
device_t *find_device(void *dfname, uint_t flags);

/**
 * @brief 创建设备中断处理
 * @param devp 设备指针
 * @param handle 中断处理函数
 * @param phyiline 物理中断线
 * @return 驱动状态码
 */
drvstus_t new_devhandle(device_t *devp, int_handler_t handle, uint_t phyiline);

/**
 * @brief 执行设备I/O
 * @param nodep 对象节点指针
 * @return 驱动状态码
 */
drvstus_t dev_io(objnode_t *nodep);

/**
 * @brief 调用驱动函数
 * @param devp 设备指针
 * @param iocode I/O控制码
 * @param val1 参数1
 * @param val2 参数2
 * @param p1 指针参数1
 * @param p2 指针参数2
 * @return 驱动状态码
 */
drvstus_t dev_call_driver(device_t *devp, uint_t iocode, uint_t val1, uint_t val2, void *p1, void *p2);

#endif // _DEVICE_H
