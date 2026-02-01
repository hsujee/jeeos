/*
 * @Author: Jee Hsu
 * @Description: PS/2 键盘驱动
 *
 * 实现标准PS/2键盘输入：
 * - 扫描码到ASCII的转换
 * - 键盘中断处理(IRQ1)
 * - 键盘缓冲区管理
 * - 特殊键处理(Shift, Ctrl, Alt等)
 *
 * 端口：
 * - 0x60: 数据端口
 * - 0x64: 状态/命令端口
 *
 * 驱动接口：
 * - keyboard_read: 读取按键
 * - keyboard_ioctrl: 控制键盘(LED等)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "drv_deps.h"

/*===========================================================================
 * 端口和常量定义
 *===========================================================================*/

#define KB_DATA_PORT    0x60    /* 键盘数据端口 */
#define KB_STATUS_PORT  0x64    /* 键盘状态端口 */
#define KB_CMD_PORT     0x64    /* 键盘命令端口 */

/* 状态寄存器标志 */
#define KB_STAT_OBF     0x01    /* 输出缓冲区满 */
#define KB_STAT_IBF     0x02    /* 输入缓冲区满 */

/* 键盘命令 */
#define KB_CMD_LED      0xED    /* 设置LED状态 */
#define KB_CMD_ECHO     0xEE    /* 回显测试 */
#define KB_CMD_RATE     0xF3    /* 设置重复率 */
#define KB_CMD_ENABLE   0xF4    /* 启用键盘 */
#define KB_CMD_RESET    0xFF    /* 复位键盘 */

/* LED状态位 */
#define KB_LED_SCROLL   0x01    /* Scroll Lock */
#define KB_LED_NUM      0x02    /* Num Lock */
#define KB_LED_CAPS     0x04    /* Caps Lock */

/* 特殊扫描码 */
#define SC_LSHIFT       0x2A
#define SC_RSHIFT       0x36
#define SC_LCTRL        0x1D
#define SC_LALT         0x38
#define SC_CAPSLOCK     0x3A
#define SC_NUMLOCK      0x45
#define SC_SCROLLLOCK   0x46
#define SC_ESCAPE       0x01
#define SC_BACKSPACE    0x0E
#define SC_TAB          0x0F
#define SC_ENTER        0x1C
#define SC_SPACE        0x39

/* 键盘缓冲区大小 */
#define KB_BUFFER_SIZE  128

/* 键盘中断号 */
#define KB_IRQ          1
#define KB_INT_VECTOR   0x21

/*===========================================================================
 * 扫描码到ASCII映射表
 *===========================================================================*/

/* 普通按键 (无Shift) */
PRIVATE const char scancode_normal[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\','z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0
};

/* Shift按键 */
PRIVATE const char scancode_shift[128] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*',  0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0
};

/*===========================================================================
 * 内部状态
 *===========================================================================*/

PRIVATE u8_t kb_buffer[KB_BUFFER_SIZE];
PRIVATE uint_t kb_head = 0;
PRIVATE uint_t kb_tail = 0;
PRIVATE uint_t kb_count = 0;

PRIVATE bool_t kb_shift = FALSE;
PRIVATE bool_t kb_ctrl = FALSE;
PRIVATE bool_t kb_alt = FALSE;
PRIVATE bool_t kb_capslock = FALSE;
PRIVATE bool_t kb_numlock = FALSE;
PRIVATE bool_t kb_scrolllock = FALSE;

PRIVATE spinlock_t kb_lock;
PRIVATE sem_t kb_sem;

/*===========================================================================
 * 内部函数
 *===========================================================================*/

/**
 * @brief 等待键盘输入缓冲区空闲
 */
PRIVATE void kb_wait_write(void) {
    while (in_u8(KB_STATUS_PORT) & KB_STAT_IBF);
}

/**
 * @brief 等待键盘输出缓冲区有数据
 */
PRIVATE bool_t kb_wait_read(void) {
    uint_t timeout = 10000;
    while (--timeout) {
        if (in_u8(KB_STATUS_PORT) & KB_STAT_OBF) {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * @brief 发送命令到键盘
 */
PRIVATE void kb_send_cmd(u8_t cmd) {
    kb_wait_write();
    out_u8(KB_DATA_PORT, cmd);
}

/**
 * @brief 更新LED状态
 */
PRIVATE void kb_update_leds(void) {
    u8_t led_status = 0;
    if (kb_scrolllock) led_status |= KB_LED_SCROLL;
    if (kb_numlock) led_status |= KB_LED_NUM;
    if (kb_capslock) led_status |= KB_LED_CAPS;
    
    kb_send_cmd(KB_CMD_LED);
    kb_wait_read();
    in_u8(KB_DATA_PORT);  /* 读取ACK */
    kb_send_cmd(led_status);
}

/**
 * @brief 向缓冲区添加字符
 */
PRIVATE void kb_put_char(u8_t c) {
    if (kb_count < KB_BUFFER_SIZE) {
        kb_buffer[kb_head] = c;
        kb_head = (kb_head + 1) % KB_BUFFER_SIZE;
        kb_count++;
        sem_up(&kb_sem);
    }
}

/**
 * @brief 从缓冲区获取字符
 */
PRIVATE sint_t kb_get_char(void) {
    if (kb_count == 0) {
        return -1;
    }
    
    u8_t c = kb_buffer[kb_tail];
    kb_tail = (kb_tail + 1) % KB_BUFFER_SIZE;
    kb_count--;
    
    return c;
}

/**
 * @brief 处理扫描码
 */
PRIVATE void kb_process_scancode(u8_t scancode) {
    bool_t is_release = (scancode & 0x80) != 0;
    u8_t code = scancode & 0x7F;
    
    /* 处理特殊键 */
    switch (code) {
    case SC_LSHIFT:
    case SC_RSHIFT:
        kb_shift = !is_release;
        return;
    case SC_LCTRL:
        kb_ctrl = !is_release;
        return;
    case SC_LALT:
        kb_alt = !is_release;
        return;
    case SC_CAPSLOCK:
        if (!is_release) {
            kb_capslock = !kb_capslock;
            kb_update_leds();
        }
        return;
    case SC_NUMLOCK:
        if (!is_release) {
            kb_numlock = !kb_numlock;
            kb_update_leds();
        }
        return;
    case SC_SCROLLLOCK:
        if (!is_release) {
            kb_scrolllock = !kb_scrolllock;
            kb_update_leds();
        }
        return;
    }
    
    /* 只处理按下事件 */
    if (is_release) {
        return;
    }
    
    /* 转换为ASCII */
    char c;
    if (kb_shift) {
        c = scancode_shift[code];
    } else {
        c = scancode_normal[code];
    }
    
    /* Caps Lock处理 */
    if (kb_capslock && c >= 'a' && c <= 'z') {
        c = c - 'a' + 'A';
    } else if (kb_capslock && c >= 'A' && c <= 'Z') {
        c = c - 'A' + 'a';
    }
    
    if (c != 0) {
        kb_put_char(c);
    }
}

/*===========================================================================
 * 驱动程序接口实现
 *===========================================================================*/

/**
 * @brief 键盘中断处理函数
 */
drvstus_t keyboard_handle(uint_t ift_nr, void *devp, void *sframe) {
    (void)ift_nr;
    (void)devp;
    (void)sframe;
    
    u8_t scancode = in_u8(KB_DATA_PORT);
    
    cpuflg_t flags;
    spinlock_cli(&kb_lock, &flags);
    kb_process_scancode(scancode);
    spinunlock_sti(&kb_lock, &flags);
    
    return DFCOKSTUS;
}

/**
 * @brief 设置驱动程序功能派发函数
 */
void keyboard_set_driver(driver_t *drvp) {
    drvp->drv_dipfun[IOIF_CODE_OPEN] = keyboard_open;
    drvp->drv_dipfun[IOIF_CODE_CLOSE] = keyboard_close;
    drvp->drv_dipfun[IOIF_CODE_READ] = keyboard_read;
    drvp->drv_dipfun[IOIF_CODE_WRITE] = keyboard_write;
    drvp->drv_dipfun[IOIF_CODE_LSEEK] = keyboard_lseek;
    drvp->drv_dipfun[IOIF_CODE_IOCTRL] = keyboard_ioctrl;
    drvp->drv_dipfun[IOIF_CODE_DEV_START] = keyboard_dev_start;
    drvp->drv_dipfun[IOIF_CODE_DEV_STOP] = keyboard_dev_stop;
    drvp->drv_dipfun[IOIF_CODE_SET_POWERSTUS] = keyboard_set_powerstus;
    drvp->drv_dipfun[IOIF_CODE_ENUM_DEV] = keyboard_enum_dev;
    drvp->drv_dipfun[IOIF_CODE_FLUSH] = keyboard_flush;
    drvp->drv_dipfun[IOIF_CODE_SHUTDOWN] = keyboard_shutdown;
    drvp->drv_name = "keyboarddrv";
    return;
}


drvstus_t keyboard_entry(driver_t *drvp, uint_t val, void *p) {
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 初始化键盘状态 */
    spinlock_init(&kb_lock);
    sem_t_init(&kb_sem);
    kb_head = 0;
    kb_tail = 0;
    kb_count = 0;
    kb_shift = FALSE;
    kb_ctrl = FALSE;
    kb_alt = FALSE;
    kb_capslock = FALSE;
    kb_numlock = TRUE;      /* 默认开启 NumLock */
    kb_scrolllock = FALSE;
    
    /* 注册带中断的设备（不会自动启用中断） */
    dev_reg_info_t info = {
        .dev_name = "keyboard",
        .dev_mtype = KEYBOARD_DEVICE,
        .dev_stype = 0,
        .dev_nr = 0,
        .dev_flags = DEVFLG_SHARE
    };
    
    device_t *devp = register_device_with_irq(
        drvp, &info, keyboard_set_driver,
        keyboard_handle, KB_INT_VECTOR
    );
    
    if (devp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 启用键盘硬件 */
    kb_send_cmd(KB_CMD_ENABLE);
    
    /* 更新LED */
    kb_update_leds();
    
    /* 硬件初始化完成后，启用键盘中断 */
    if (enable_intline(KB_INT_VECTOR) == DFCERRSTUS) {
        return DFCERRSTUS;
    }
    
    return DFCOKSTUS;
}

/**
 * @brief 驱动程序退出函数
 */
drvstus_t keyboard_exit(driver_t *drvp, uint_t val, void *p) {
    return DFCOKSTUS;
}

drvstus_t keyboard_open(device_t *devp, void *iopack) {
    return device_default_open(devp, iopack);
}

drvstus_t keyboard_close(device_t *devp, void *iopack) {
    return device_default_close(devp, iopack);
}

/**
 * @brief 读取按键
 */
drvstus_t keyboard_read(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL || obp->on_len == 0) {
        return DFCERRSTUS;
    }
    
    u8_t *buf = (u8_t *)obp->on_buf;
    size_t count = 0;
    
    cpuflg_t flags;
    spinlock_cli(&kb_lock, &flags);
    
    while (count < obp->on_len && kb_count > 0) {
        sint_t c = kb_get_char();
        if (c >= 0) {
            buf[count++] = (u8_t)c;
        }
    }
    
    spinunlock_sti(&kb_lock, &flags);
    
    obp->on_len = count;
    
    return DFCOKSTUS;
}

/**
 * @brief 写入设备 (不支持)
 */
drvstus_t keyboard_write(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

/**
 * @brief 调整位置 (不支持)
 */
drvstus_t keyboard_lseek(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

/**
 * @brief 设备控制
 */
drvstus_t keyboard_ioctrl(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 启动设备
 */
drvstus_t keyboard_dev_start(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 停止设备
 */
drvstus_t keyboard_dev_stop(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 设置电源状态
 */
drvstus_t keyboard_set_powerstus(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 枚举设备
 */
drvstus_t keyboard_enum_dev(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 刷新缓存
 */
drvstus_t keyboard_flush(device_t *devp, void *iopack) {
    cpuflg_t flags;
    spinlock_cli(&kb_lock, &flags);
    kb_head = 0;
    kb_tail = 0;
    kb_count = 0;
    spinunlock_sti(&kb_lock, &flags);
    return DFCOKSTUS;
}

/**
 * @brief 关闭设备电源
 */
drvstus_t keyboard_shutdown(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

