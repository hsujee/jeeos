/*
 * @Author: Jee Hsu
 * @Description: UART串口驱动(16550兼容)
 *
 * 管理串行端口通信：
 * - COM1: 0x3F8 (IRQ4)
 * - COM2: 0x2F8 (IRQ3)
 *
 * 功能特性：
 * - 波特率配置(默认115200)
 * - 数据位/停止位/校验位设置
 * - 轮询模式读写
 *
 * 驱动接口：
 * - uart_open: 初始化串口
 * - uart_read: 读取数据
 * - uart_write: 发送数据
 * - uart_ioctrl: 配置参数
 *
 * 用途：
 * - 调试输出
 * - 串口终端
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "drv_deps.h"

/*===========================================================================
 * UART 寄存器定义 (16550 兼容)
 *===========================================================================*/

#define COM1_PORT           0x3F8   /* COM1 基地址 */
#define COM2_PORT           0x2F8   /* COM2 基地址 */

/* 寄存器偏移 */
#define UART_THR            0       /* 发送保持寄存器 (写) */
#define UART_RBR            0       /* 接收缓冲寄存器 (读) */
#define UART_IER            1       /* 中断使能寄存器 */
#define UART_IIR            2       /* 中断标识寄存器 (读) */
#define UART_FCR            2       /* FIFO控制寄存器 (写) */
#define UART_LCR            3       /* 线路控制寄存器 */
#define UART_MCR            4       /* 调制解调器控制寄存器 */
#define UART_LSR            5       /* 线路状态寄存器 */
#define UART_MSR            6       /* 调制解调器状态寄存器 */
#define UART_SCR            7       /* 暂存寄存器 */

/* 除数锁存寄存器 (DLAB=1时) */
#define UART_DLL            0       /* 除数锁存低字节 */
#define UART_DLH            1       /* 除数锁存高字节 */

/* 线路状态寄存器位 */
#define UART_LSR_DR         0x01    /* 数据就绪 */
#define UART_LSR_OE         0x02    /* 溢出错误 */
#define UART_LSR_PE         0x04    /* 奇偶校验错误 */
#define UART_LSR_FE         0x08    /* 帧错误 */
#define UART_LSR_BI         0x10    /* 中断指示 */
#define UART_LSR_THRE       0x20    /* 发送保持寄存器空 */
#define UART_LSR_TEMT       0x40    /* 发送器空 */
#define UART_LSR_FIFOE      0x80    /* FIFO错误 */

/* 线路控制寄存器位 */
#define UART_LCR_DLAB       0x80    /* 除数锁存访问位 */
#define UART_LCR_8N1        0x03    /* 8数据位, 无校验, 1停止位 */

/* 波特率除数 (基于1.8432MHz时钟) */
#define UART_BAUD_115200    1       /* 115200 bps */
#define UART_BAUD_57600     2       /* 57600 bps */
#define UART_BAUD_38400     3       /* 38400 bps */
#define UART_BAUD_19200     6       /* 19200 bps */
#define UART_BAUD_9600      12      /* 9600 bps */

/* 默认使用 COM1 */
static u16_t uart_base_port = COM1_PORT;

/*===========================================================================
 * 硬件操作函数
 *===========================================================================*/

/**
 * @brief 初始化 UART 硬件
 * @param port 端口基地址
 * @param baud_divisor 波特率除数
 */
static void init_uart_hardware(u16_t port, u16_t baud_divisor) {
    /* 禁用所有中断 */
    out_u8(port + UART_IER, 0x00);
    
    /* 启用 DLAB 以设置波特率 */
    out_u8(port + UART_LCR, UART_LCR_DLAB);
    
    /* 设置波特率除数 */
    out_u8(port + UART_DLL, (u8_t)(baud_divisor & 0xFF));
    out_u8(port + UART_DLH, (u8_t)((baud_divisor >> 8) & 0xFF));
    
    /* 设置 8N1 模式 (8数据位, 无校验, 1停止位) */
    out_u8(port + UART_LCR, UART_LCR_8N1);
    
    /* 启用 FIFO, 清空接收和发送 FIFO, 设置14字节触发 */
    out_u8(port + UART_FCR, 0xC7);
    
    /* 启用 DTR, RTS, OUT2 (用于中断) */
    out_u8(port + UART_MCR, 0x0B);
    
    /* 清空接收缓冲区 */
    while (in_u8(port + UART_LSR) & UART_LSR_DR) {
        in_u8(port + UART_RBR);
    }
}

/**
 * @brief 检测 UART 是否存在
 * @param port 端口基地址
 * @return TRUE 存在, FALSE 不存在
 */
static bool_t uart_detect(u16_t port) {
    /* 写入暂存寄存器并读回验证 */
    out_u8(port + UART_SCR, 0xAA);
    if (in_u8(port + UART_SCR) != 0xAA) {
        return FALSE;
    }
    out_u8(port + UART_SCR, 0x55);
    if (in_u8(port + UART_SCR) != 0x55) {
        return FALSE;
    }
    return TRUE;
}

/**
 * @brief 发送单个字符
 * @param port 端口基地址
 * @param c 字符
 */
static void uart_putchar(u16_t port, char c) {
    /* 等待发送保持寄存器空 */
    while ((in_u8(port + UART_LSR) & UART_LSR_THRE) == 0);
    /* 发送字符 */
    out_u8(port + UART_THR, c);
}

/**
 * @brief 接收单个字符 (非阻塞)
 * @param port 端口基地址
 * @param c 字符指针
 * @return TRUE 成功, FALSE 无数据
 */
static bool_t uart_getchar(u16_t port, char *c) {
    /* 检查是否有数据就绪 */
    if ((in_u8(port + UART_LSR) & UART_LSR_DR) == 0) {
        return FALSE;
    }
    /* 读取字符 */
    *c = (char)in_u8(port + UART_RBR);
    return TRUE;
}

/*===========================================================================
 * 驱动程序接口
 *===========================================================================*/

void uart_set_driver(driver_t *drvp) {
    drvp->drv_dipfun[IOIF_CODE_OPEN] = uart_open;
    drvp->drv_dipfun[IOIF_CODE_CLOSE] = uart_close;
    drvp->drv_dipfun[IOIF_CODE_READ] = uart_read;
    drvp->drv_dipfun[IOIF_CODE_WRITE] = uart_write;
    drvp->drv_dipfun[IOIF_CODE_LSEEK] = uart_lseek;
    drvp->drv_dipfun[IOIF_CODE_IOCTRL] = uart_ioctrl;
    drvp->drv_dipfun[IOIF_CODE_DEV_START] = uart_dev_start;
    drvp->drv_dipfun[IOIF_CODE_DEV_STOP] = uart_dev_stop;
    drvp->drv_dipfun[IOIF_CODE_SET_POWERSTUS] = uart_set_powerstus;
    drvp->drv_dipfun[IOIF_CODE_ENUM_DEV] = uart_enum_dev;
    drvp->drv_dipfun[IOIF_CODE_FLUSH] = uart_flush;
    drvp->drv_dipfun[IOIF_CODE_SHUTDOWN] = uart_shutdown;
    drvp->drv_name = "uartdrv";
    return;
}


drvstus_t uart_entry(driver_t *drvp, uint_t val, void *p) {
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 检测 UART 硬件 */
    if (!uart_detect(COM1_PORT)) {
        return DFCERRSTUS;
    }
    
    /* 初始化 UART 硬件 (115200 8N1) */
    init_uart_hardware(COM1_PORT, UART_BAUD_115200);
    uart_base_port = COM1_PORT;
    
    /* 注册设备 */
    dev_reg_info_t info = {
        .dev_name = "uart0",
        .dev_mtype = UART_DEVICE,
        .dev_stype = 0,
        .dev_nr = 0,
        .dev_flags = DEVFLG_SHARE
    };
    
    device_t *devp = register_device_common(drvp, &info, uart_set_driver);
    if (devp == NULL) {
        return DFCERRSTUS;
    }

    return DFCOKSTUS;
}

drvstus_t uart_exit(driver_t *drvp, uint_t val, void *p) {
    return DFCERRSTUS;
}

drvstus_t uart_open(device_t *devp, void *iopack) {
    return device_default_open(devp, iopack);
}

drvstus_t uart_close(device_t *devp, void *iopack) {
    return device_default_close(devp, iopack);
}

/**
 * @brief 从串口读取数据
 * @param devp 设备指针
 * @param iopack I/O请求包
 * @return DFCOKSTUS 成功, DFCERRSTUS 失败
 */
drvstus_t uart_read(device_t *devp, void *iopack) {
    uint_t len;
    buf_t retbuf;
    
    if (dev_retn_rqueparm(iopack, &retbuf, NULL, &len, NULL, NULL, NULL) == DFCERRSTUS) {
        return DFCERRSTUS;
    }
    
    if (retbuf == NULL || len == 0) {
        return DFCERRSTUS;
    }
    
    char *buf = (char *)retbuf;
    uint_t count = 0;
    
    /* 读取可用数据 (非阻塞) */
    while (count < len) {
        char c;
        if (!uart_getchar(uart_base_port, &c)) {
            break;  /* 无更多数据 */
        }
        buf[count++] = c;
    }
    
    /* 更新实际读取长度 */
    objnode_t *obp = (objnode_t *)iopack;
    obp->on_len = count;
    
    return DFCOKSTUS;
}

/**
 * @brief 向串口写入数据
 * @param devp 设备指针
 * @param iopack I/O请求包
 * @return DFCOKSTUS 成功, DFCERRSTUS 失败
 */
drvstus_t uart_write(device_t *devp, void *iopack) {
    uint_t len;
    buf_t retbuf;
    
    if (dev_retn_rqueparm(iopack, &retbuf, NULL, &len, NULL, NULL, NULL) == DFCERRSTUS) {
        return DFCERRSTUS;
    }
    
    if (retbuf == NULL) {
        return DFCERRSTUS;
    }
    
    char *p = (char *)retbuf;
    
    /* 直接操作硬件发送数据 */
    for (uint_t i = 0; i < len && p[i] != '\0'; i++) {
        /* 处理换行符 */
        if (p[i] == '\n') {
            uart_putchar(uart_base_port, '\r');
        }
        uart_putchar(uart_base_port, p[i]);
    }
    
    return DFCOKSTUS;
}

drvstus_t uart_lseek(device_t *devp, void *iopack) {
    /* 串口不支持 seek 操作 */
    return DFCERRSTUS;
}

/**
 * @brief 串口控制
 * @param devp 设备指针
 * @param iopack I/O请求包
 * @return DFCOKSTUS 成功, DFCERRSTUS 失败
 * 
 * 支持的控制码:
 * - 1: 设置波特率 (buf指向波特率值)
 */
drvstus_t uart_ioctrl(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    switch (obp->on_ioctrd) {
        case 1: {  /* 设置波特率 */
            if (obp->on_buf == NULL) {
                return DFCERRSTUS;
            }
            uint_t baud = *(uint_t *)obp->on_buf;
            u16_t divisor;
            
            switch (baud) {
                case 115200: divisor = UART_BAUD_115200; break;
                case 57600:  divisor = UART_BAUD_57600;  break;
                case 38400:  divisor = UART_BAUD_38400;  break;
                case 19200:  divisor = UART_BAUD_19200;  break;
                case 9600:   divisor = UART_BAUD_9600;   break;
                default:     return DFCERRSTUS;
            }
            
            init_uart_hardware(uart_base_port, divisor);
            return DFCOKSTUS;
        }
        default:
            return DFCERRSTUS;
    }
}

drvstus_t uart_dev_start(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

drvstus_t uart_dev_stop(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

drvstus_t uart_set_powerstus(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

drvstus_t uart_enum_dev(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

drvstus_t uart_flush(device_t *devp, void *iopack) {
    /* 等待发送完成 */
    while ((in_u8(uart_base_port + UART_LSR) & UART_LSR_TEMT) == 0);
    return DFCOKSTUS;
}

drvstus_t uart_shutdown(device_t *devp, void *iopack) {
    /* 禁用中断 */
    out_u8(uart_base_port + UART_IER, 0x00);
    return DFCOKSTUS;
}
