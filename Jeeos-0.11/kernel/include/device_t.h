/*
 * @Author: Jee Hsu
 * @Description: 设备和驱动程序类型定义
 *
 * 定义内核设备管理框架的核心数据结构：
 * - device_t: 设备描述符
 * - driver_t: 驱动程序描述符
 * - devtable_t: 全局设备表
 * - devid_t: 设备ID
 *
 * 设备驱动模型采用分层架构：
 *   设备表 -> 设备类型列表 -> 设备 -> 驱动程序
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef DEVICE_T_H
#define DEVICE_T_H

/** @name 设备类型定义 */
/** @{ */
#define NOT_DEVICE 0            /**< 无设备 */
#define AMBABUS_DEVICE 1        /**< AMBA总线设备 */
#define AHBBUS_DEVICE 2         /**< AHB总线设备 */
#define APBBUS_DEVICE 3         /**< APB总线设备 */
#define BRIDGE_DEVICE 4         /**< 总线桥接器 */
#define CPUCORE_DEVICE 5        /**< CPU核心 */
#define RAMCONTER_DEVICE 6      /**< 内存控制器 */
#define RAM_DEVICE 7            /**< 内存设备 */
#define USBHOSTCONTER_DEVICE 8  /**< USB主控制器 */
#define INTUPTCONTER_DEVICE 9   /**< 中断控制器 */
#define DMA_DEVICE 10           /**< DMA控制器 */
#define CLOCKPOWER_DEVICE 11    /**< 时钟电源管理 */
#define LCDCONTER_DEVICE 12     /**< LCD控制器 */
#define NANDFLASH_DEVICE 13     /**< NAND Flash */
#define CAMERA_DEVICE 14        /**< 摄像头 */
#define UART_DEVICE 15          /**< 串口 */
#define TIMER_DEVICE 16         /**< 定时器 */
#define USB_DEVICE 17           /**< USB设备 */
#define WATCHDOG_DEVICE 18      /**< 看门狗 */
#define IIC_DEVICE 19           /**< I2C设备 */
#define IIS_DEVICE 20           /**< I2S音频 */
#define GPIO_DEVICE 21          /**< GPIO */
#define RTC_DEVICE 22           /**< 实时时钟 */
#define A_DCONVER_DEVICE 23     /**< ADC转换器 */
#define SPI_DEVICE 24           /**< SPI设备 */
#define SD_DEVICE 25            /**< SD卡 */
#define AUDIO_DEVICE 26         /**< 音频设备 */
#define TOUCH_DEVICE 27         /**< 触控设备 */
#define NETWORK_DEVICE 28       /**< 网络设备 */
#define VIR_DEVICE 29           /**< 虚拟设备 */
#define FILESYS_DEVICE 30       /**< 文件系统设备 */
#define SYSTICK_DEVICE 31       /**< 系统时钟 */
#define UNKNOWN_DEVICE 32       /**< 未知设备 */
#define HD_DEVICE 33            /**< 硬盘设备 */
#define DEVICE_MAX 34           /**< 设备类型数量 */
/** @} */

/** @name I/O操作码定义 */
/** @{ */
#define IOIF_CODE_OPEN 0            /**< 打开设备 */
#define IOIF_CODE_CLOSE 1           /**< 关闭设备 */
#define IOIF_CODE_READ 2            /**< 读取数据 */
#define IOIF_CODE_WRITE 3           /**< 写入数据 */
#define IOIF_CODE_LSEEK 4           /**< 调整位置 */
#define IOIF_CODE_IOCTRL 5          /**< 设备控制 */
#define IOIF_CODE_DEV_START 6       /**< 启动设备 */
#define IOIF_CODE_DEV_STOP 7        /**< 停止设备 */
#define IOIF_CODE_SET_POWERSTUS 8   /**< 电源管理 */
#define IOIF_CODE_ENUM_DEV 9        /**< 枚举设备 */
#define IOIF_CODE_FLUSH 10          /**< 刷新缓冲 */
#define IOIF_CODE_SHUTDOWN 11       /**< 关闭系统 */
#define IOIF_CODE_MAX 12            /**< 操作码数量 */
/** @} */

/** @name 设备标志和状态 */
/** @{ */
#define DEVFLG_EXCLU (1<<0)         /**< 独占模式 */
#define DEVFLG_SHARE (1<<1)         /**< 共享模式 */
#define DEVSTS_NORML (1<<0)         /**< 正常状态 */
#define DEVSTS_FAILU (1<<1)         /**< 故障状态 */
#define DIDFIL_IDN 1                /**< 按ID查找 */
#define DIDFIL_FLN 2                /**< 按名称查找 */
/** @} */

/** @name 文件系统设备标志 */
/** @{ */
#define FSDEV_IOCTRCD_DELFILE 5     /**< 删除文件控制码 */
#define FSDEV_OPENFLG_NEWFILE 1     /**< 创建新文件 */
#define FSDEV_OPENFLG_OPEFILE 2     /**< 打开已有文件 */
/** @} */

/**
 * @brief 设备ID结构
 * 
 * 唯一标识系统中的每个设备
 */
typedef struct s_DEVID {
    uint_t dev_mtype;       /**< 主设备类型号 */
    uint_t dev_stype;       /**< 子设备类型号 */
    uint_t dev_nr;          /**< 设备序号 */
} devid_t;

/**
 * @brief 设备类型列表结构
 * 
 * 管理同一类型的所有设备
 */
typedef struct s_DEVTLST {
    uint_t dtl_type;        /**< 设备类型 */
    uint_t dtl_nr;          /**< 该类型设备数量 */
    list_h_t dtl_list;      /**< 设备链表头 */
} devtlst_t;

/**
 * @brief 全局设备表结构
 * 
 * 系统唯一的设备管理中心
 */
typedef struct s_DEVTABLE {
    list_h_t devt_list;     /**< 设备表链表节点 */
    spinlock_t devt_lock;   /**< 保护设备表的自旋锁 */
    list_h_t devt_devlist;  /**< 所有设备的链表 */
    list_h_t devt_drvlist;  /**< 所有驱动的链表 */
    uint_t devt_devnr;      /**< 设备总数 */
    uint_t devt_drvnr;      /**< 驱动总数 */
    devtlst_t devt_devclsl[DEVICE_MAX]; /**< 按类型分类的设备列表 */
} devtable_t;

/**
 * @brief 设备描述符结构
 * 
 * 代表系统中的一个物理或虚拟设备
 */
typedef struct s_DEVICE {
    list_h_t dev_list;      /**< 全局设备链表节点 */
    list_h_t dev_indrvlst;  /**< 在驱动程序中的链表节点 */
    list_h_t dev_intbllst;  /**< 在设备表中的链表节点 */
    spinlock_t dev_lock;    /**< 保护设备的自旋锁 */
    uint_t dev_count;       /**< 打开计数 */
    sem_t dev_sem;          /**< 设备访问信号量 */
    uint_t dev_status;        /**< 设备状态 */
    uint_t dev_flags;       /**< 设备标志 */
    devid_t dev_id;         /**< 设备ID */
    uint_t dev_intlnenr;    /**< 中断服务例程数量 */
    list_h_t dev_intserlst; /**< 中断服务例程链表 */
    list_h_t dev_rqlist;    /**< 请求队列 */
    uint_t dev_rqlnr;       /**< 请求数量 */
    sem_t dev_waitints;     /**< 等待中断信号量 */
    struct s_DRIVER* dev_drv; /**< 关联的驱动程序 */
    void* dev_attrb;        /**< 设备属性 */
    void* dev_privdata;     /**< 私有数据 */
    void* dev_userdata;     /**< 用户数据(扩展) */
    void* dev_extdata;      /**< 扩展数据 */
    char_t* dev_name;       /**< 设备名称 */
} device_t;

/** 驱动程序分派函数类型 */
typedef drvstus_t (*drivcallfun_t)(device_t*, void*);
/** 驱动程序入口/退出函数类型 */
typedef drvstus_t (*drventyexit_t)(struct s_DRIVER*, uint_t, void*);

/**
 * @brief 驱动程序描述符结构
 * 
 * 代表一个设备驱动程序，管理一类设备的操作
 */
typedef struct s_DRIVER {
    spinlock_t drv_lock;    /**< 保护驱动的自旋锁 */
    list_h_t drv_list;      /**< 驱动链表节点 */
    uint_t drv_status;       /**< 驱动状态 */
    uint_t drv_flags;       /**< 驱动标志 */
    uint_t drv_id;          /**< 驱动ID */
    uint_t drv_count;       /**< 引用计数 */
    sem_t drv_sem;          /**< 驱动信号量 */
    void* drv_safedsc;      /**< 安全描述符 */
    void* drv_attrb;        /**< 驱动属性 */
    void* drv_privdata;     /**< 私有数据 */
    drivcallfun_t drv_dipfun[IOIF_CODE_MAX]; /**< 操作分派函数表 */
    list_h_t drv_alldevlist; /**< 管理的设备链表 */
    drventyexit_t drv_entry; /**< 驱动入口函数 */
    drventyexit_t drv_exit;  /**< 驱动退出函数 */
    void* drv_userdata;     /**< 用户数据(扩展) */
    void* drv_extdata;      /**< 扩展数据 */
    char_t* drv_name;       /**< 驱动名称 */
} driver_t;

#endif // DEVICE_T_H
