/*
 * @Author: Jee Hsu
 * @Description: VGA 文本模式驱动
 *
 * 提供VGA文本模式显示功能：
 * - 80x25文本模式
 * - 16色前景/背景
 * - 光标控制
 * - 屏幕滚动
 *
 * 内存映射：
 * - 0xB8000: 文本模式显存起始地址
 * - 每个字符占2字节: 字符 + 属性
 *
 * 驱动接口：
 * - vga_write: 输出字符串
 * - vga_ioctrl: 控制光标、清屏、设置颜色
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "drv_deps.h"

/*===========================================================================
 * 常量定义
 *===========================================================================*/

#define VGA_MEMORY_PHYS 0xB8000     /* VGA文本模式显存物理地址 */
#define VGA_COLS        80          /* 列数 */
#define VGA_ROWS        25          /* 行数 */
#define VGA_SIZE        (VGA_COLS * VGA_ROWS)

/* VGA I/O端口 */
#define VGA_CTRL_PORT   0x3D4       /* 控制寄存器端口 */
#define VGA_DATA_PORT   0x3D5       /* 数据端口 */

/* 光标寄存器索引 */
#define VGA_CURSOR_HIGH 0x0E        /* 光标位置高字节 */
#define VGA_CURSOR_LOW  0x0F        /* 光标位置低字节 */

/* 颜色定义 */
#define VGA_BLACK       0
#define VGA_BLUE        1
#define VGA_GREEN       2
#define VGA_CYAN        3
#define VGA_RED         4
#define VGA_MAGENTA     5
#define VGA_BROWN       6
#define VGA_LIGHTGREY   7
#define VGA_DARKGREY    8
#define VGA_LIGHTBLUE   9
#define VGA_LIGHTGREEN  10
#define VGA_LIGHTCYAN   11
#define VGA_LIGHTRED    12
#define VGA_LIGHTMAGENTA 13
#define VGA_YELLOW      14
#define VGA_WHITE       15

/* ioctl 命令 */
#define VGA_IOCTL_CLEAR         1   /* 清屏 */
#define VGA_IOCTL_SET_CURSOR    2   /* 设置光标位置 */
#define VGA_IOCTL_GET_CURSOR    3   /* 获取光标位置 */
#define VGA_IOCTL_SET_COLOR     4   /* 设置颜色 */
#define VGA_IOCTL_SCROLL_UP     5   /* 向上滚动 */
#define VGA_IOCTL_SCROLL_DOWN   6   /* 向下滚动 */
#define VGA_IOCTL_SET_TEXTMODE  7   /* 切换到文本模式 */

/*===========================================================================
 * VGA位置结构
 *===========================================================================*/

typedef struct {
    u16_t row;      /* 行 (0-24) */
    u16_t col;      /* 列 (0-79) */
} vga_pos_t;

/*===========================================================================
 * 内部状态
 *===========================================================================*/

PRIVATE u16_t *vga_buffer = NULL;   /* 在驱动初始化时设置为虚拟地址 */
PRIVATE u16_t vga_cursor_row = 0;
PRIVATE u16_t vga_cursor_col = 0;
PRIVATE u8_t vga_color = 0x07;      /* 默认: 灰字黑底 */
PRIVATE bool_t vga_text_mode = FALSE;  /* 是否处于文本模式 */

/*===========================================================================
 * VGA 寄存器端口
 *===========================================================================*/

#define VGA_MISC_WRITE      0x3C2   /* 杂项输出寄存器(写) */
#define VGA_MISC_READ       0x3CC   /* 杂项输出寄存器(读) */
#define VGA_SEQ_INDEX       0x3C4   /* 序列控制器索引 */
#define VGA_SEQ_DATA        0x3C5   /* 序列控制器数据 */
#define VGA_GC_INDEX        0x3CE   /* 图形控制器索引 */
#define VGA_GC_DATA         0x3CF   /* 图形控制器数据 */
#define VGA_CRTC_INDEX      0x3D4   /* CRT控制器索引 */
#define VGA_CRTC_DATA       0x3D5   /* CRT控制器数据 */
#define VGA_AC_INDEX        0x3C0   /* 属性控制器索引 */
#define VGA_AC_WRITE        0x3C0   /* 属性控制器写 */
#define VGA_AC_READ         0x3C1   /* 属性控制器读 */
#define VGA_INSTAT_READ     0x3DA   /* 输入状态寄存器 */

/*===========================================================================
 * VGA 文本模式寄存器值 (80x25, 16色)
 *===========================================================================*/

/* 杂项输出寄存器 */
PRIVATE const u8_t vga_mode3_misc = 0x67;

/* 序列控制器寄存器 (索引0-4) */
PRIVATE const u8_t vga_mode3_seq[] = {
    0x03, 0x00, 0x03, 0x00, 0x02
};

/* CRTC 寄存器 (索引0-24) */
PRIVATE const u8_t vga_mode3_crtc[] = {
    0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
    0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
    0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
    0xFF
};

/* 图形控制器寄存器 (索引0-8) */
PRIVATE const u8_t vga_mode3_gc[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00, 0xFF
};

/* 属性控制器寄存器 (索引0-20) */
PRIVATE const u8_t vga_mode3_ac[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x0C, 0x00, 0x0F, 0x08, 0x00
};

/*===========================================================================
 * 内部函数
 *===========================================================================*/

/**
 * @brief 切换到 VGA 文本模式 (80x25)
 */
PRIVATE void vga_set_text_mode(void) {
    uint_t i;
    
    /* 1. 写入杂项输出寄存器 */
    out_u8(VGA_MISC_WRITE, vga_mode3_misc);
    
    /* 2. 写入序列控制器寄存器 */
    for (i = 0; i < 5; i++) {
        out_u8(VGA_SEQ_INDEX, (u8_t)i);
        out_u8(VGA_SEQ_DATA, vga_mode3_seq[i]);
    }
    
    /* 3. 解锁 CRTC 寄存器 */
    out_u8(VGA_CRTC_INDEX, 0x03);
    out_u8(VGA_CRTC_DATA, in_u8(VGA_CRTC_DATA) | 0x80);
    out_u8(VGA_CRTC_INDEX, 0x11);
    out_u8(VGA_CRTC_DATA, in_u8(VGA_CRTC_DATA) & ~0x80);
    
    /* 4. 写入 CRTC 寄存器 */
    for (i = 0; i < 25; i++) {
        out_u8(VGA_CRTC_INDEX, (u8_t)i);
        out_u8(VGA_CRTC_DATA, vga_mode3_crtc[i]);
    }
    
    /* 5. 写入图形控制器寄存器 */
    for (i = 0; i < 9; i++) {
        out_u8(VGA_GC_INDEX, (u8_t)i);
        out_u8(VGA_GC_DATA, vga_mode3_gc[i]);
    }
    
    /* 6. 写入属性控制器寄存器 */
    in_u8(VGA_INSTAT_READ);  /* 复位属性控制器触发器 */
    for (i = 0; i < 21; i++) {
        out_u8(VGA_AC_INDEX, (u8_t)i);
        out_u8(VGA_AC_WRITE, vga_mode3_ac[i]);
    }
    
    /* 7. 启用视频输出 */
    in_u8(VGA_INSTAT_READ);
    out_u8(VGA_AC_INDEX, 0x20);
    
    vga_text_mode = TRUE;
}

/**
 * @brief 生成VGA颜色属性
 */
PRIVATE u8_t vga_make_color(u8_t fg, u8_t bg) {
    return (bg << 4) | (fg & 0x0F);
}

/**
 * @brief 生成VGA字符条目
 */
PRIVATE u16_t vga_make_entry(char c, u8_t color) {
    return (u16_t)c | ((u16_t)color << 8);
}

/**
 * @brief 更新硬件光标
 */
PRIVATE void vga_update_cursor(void) {
    u16_t pos = (u16_t)(vga_cursor_row * VGA_COLS + vga_cursor_col);
    
    out_u8(VGA_CTRL_PORT, VGA_CURSOR_HIGH);
    out_u8(VGA_DATA_PORT, (u8_t)((pos >> 8) & 0xFF));
    out_u8(VGA_CTRL_PORT, VGA_CURSOR_LOW);
    out_u8(VGA_DATA_PORT, (u8_t)(pos & 0xFF));
}

/**
 * @brief 滚动屏幕
 */
PRIVATE void vga_scroll(void) {
    if (vga_cursor_row >= VGA_ROWS) {
        /* 向上滚动一行 */
        for (uint_t i = 0; i < (VGA_ROWS - 1) * VGA_COLS; i++) {
            vga_buffer[i] = vga_buffer[i + VGA_COLS];
        }
        
        /* 清空最后一行 */
        u16_t blank = vga_make_entry(' ', vga_color);
        for (uint_t i = (VGA_ROWS - 1) * VGA_COLS; i < VGA_SIZE; i++) {
            vga_buffer[i] = blank;
        }
        
        vga_cursor_row = VGA_ROWS - 1;
    }
}

/**
 * @brief 输出单个字符
 */
PRIVATE void vga_putchar(char c) {
    switch (c) {
    case '\n':
        vga_cursor_col = 0;
        vga_cursor_row++;
        break;
    case '\r':
        vga_cursor_col = 0;
        break;
    case '\t':
        vga_cursor_col = (vga_cursor_col + 8) & ~7;
        if (vga_cursor_col >= VGA_COLS) {
            vga_cursor_col = 0;
            vga_cursor_row++;
        }
        break;
    case '\b':
        if (vga_cursor_col > 0) {
            vga_cursor_col--;
            vga_buffer[vga_cursor_row * VGA_COLS + vga_cursor_col] = 
                vga_make_entry(' ', vga_color);
        }
        break;
    default:
        if (c >= ' ') {
            vga_buffer[vga_cursor_row * VGA_COLS + vga_cursor_col] = 
                vga_make_entry(c, vga_color);
            vga_cursor_col++;
            if (vga_cursor_col >= VGA_COLS) {
                vga_cursor_col = 0;
                vga_cursor_row++;
            }
        }
        break;
    }
    
    vga_scroll();
}

/**
 * @brief 清屏
 */
PRIVATE void vga_clear(void) {
    u16_t blank = vga_make_entry(' ', vga_color);
    for (uint_t i = 0; i < VGA_SIZE; i++) {
        vga_buffer[i] = blank;
    }
    vga_cursor_row = 0;
    vga_cursor_col = 0;
    vga_update_cursor();
}

/*===========================================================================
 * 驱动程序接口实现
 *===========================================================================*/

/**
 * @brief 设置驱动程序功能派发函数
 */
void vga_set_driver(driver_t *drvp) {
    drvp->drv_dipfun[IOIF_CODE_OPEN] = vga_open;
    drvp->drv_dipfun[IOIF_CODE_CLOSE] = vga_close;
    drvp->drv_dipfun[IOIF_CODE_READ] = vga_read;
    drvp->drv_dipfun[IOIF_CODE_WRITE] = vga_write;
    drvp->drv_dipfun[IOIF_CODE_LSEEK] = vga_lseek;
    drvp->drv_dipfun[IOIF_CODE_IOCTRL] = vga_ioctrl;
    drvp->drv_dipfun[IOIF_CODE_DEV_START] = vga_dev_start;
    drvp->drv_dipfun[IOIF_CODE_DEV_STOP] = vga_dev_stop;
    drvp->drv_dipfun[IOIF_CODE_SET_POWERSTUS] = vga_set_powerstus;
    drvp->drv_dipfun[IOIF_CODE_ENUM_DEV] = vga_enum_dev;
    drvp->drv_dipfun[IOIF_CODE_FLUSH] = vga_flush;
    drvp->drv_dipfun[IOIF_CODE_SHUTDOWN] = vga_shutdown;
    drvp->drv_name = "vgadrv";
    return;
}

/**
 * @brief 设置设备属性
 */
void vga_set_device(device_t *devp, driver_t *drvp) {
    devp->dev_flags = DEVFLG_SHARE;
    devp->dev_status = DEVSTS_NORML;
    devp->dev_id.dev_mtype = VGA_DEVICE;
    devp->dev_id.dev_stype = 0;
    devp->dev_id.dev_nr = 0;
    devp->dev_name = "vga";
    return;
}

/**
 * @brief 驱动程序入口函数
 */
drvstus_t vga_entry(driver_t *drvp, uint_t val, void *p) {
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    
    device_t *devp = new_device_dsc();
    if (devp == NULL) {
        return DFCERRSTUS;
    }
    
    vga_set_driver(drvp);
    vga_set_device(devp, drvp);
    
    if (dev_add_driver(devp, drvp) == DFCERRSTUS) {
        del_device_dsc(devp);
        return DFCERRSTUS;
    }
    
    if (new_device(devp) == DFCERRSTUS) {
        del_device_dsc(devp);
        return DFCERRSTUS;
    }
    
    /* 初始化 VGA 显存虚拟地址 */
    vga_buffer = (u16_t *)phyadr_to_viradr(VGA_MEMORY_PHYS);
    
    /* 注意：不在驱动加载时切换到文本模式，由 shell 主动切换 */
    
    /* 初始化光标 */
    vga_update_cursor();
    
    printk("  [VGA] Text mode driver loaded (buffer at %x)\n", (u64_t)vga_buffer);
    
    return DFCOKSTUS;
}

/**
 * @brief 驱动程序退出函数
 */
drvstus_t vga_exit(driver_t *drvp, uint_t val, void *p) {
    return DFCOKSTUS;
}

/**
 * @brief 打开设备
 */
drvstus_t vga_open(device_t *devp, void *iopack) {
    return dev_inc_devcount(devp);
}

/**
 * @brief 关闭设备
 */
drvstus_t vga_close(device_t *devp, void *iopack) {
    return dev_dec_devcount(devp);
}

/**
 * @brief 读取设备 (读取当前屏幕内容)
 */
drvstus_t vga_read(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL) {
        return DFCERRSTUS;
    }
    
    size_t len = obp->on_len;
    if (len > VGA_SIZE * 2) {
        len = VGA_SIZE * 2;
    }
    
    memcopy((void *)vga_buffer, obp->on_buf, len);
    obp->on_len = len;
    
    return DFCOKSTUS;
}

/**
 * @brief 写入字符串
 */
drvstus_t vga_write(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL) {
        return DFCERRSTUS;
    }
    
    char *str = (char *)obp->on_buf;
    for (size_t i = 0; i < obp->on_len && str[i]; i++) {
        vga_putchar(str[i]);
    }
    
    vga_update_cursor();
    
    return DFCOKSTUS;
}

/**
 * @brief 调整位置 (设置光标位置)
 */
drvstus_t vga_lseek(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    u32_t pos = (u32_t)obp->on_currops;
    vga_cursor_row = (u16_t)(pos / VGA_COLS);
    vga_cursor_col = (u16_t)(pos % VGA_COLS);
    
    if (vga_cursor_row >= VGA_ROWS) vga_cursor_row = VGA_ROWS - 1;
    
    vga_update_cursor();
    
    return DFCOKSTUS;
}

/**
 * @brief 设备控制
 */
drvstus_t vga_ioctrl(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    switch (obp->on_ioctrd) {
    case VGA_IOCTL_CLEAR:
        vga_clear();
        break;
    case VGA_IOCTL_SET_CURSOR:
        if (obp->on_buf && obp->on_len >= sizeof(vga_pos_t)) {
            vga_pos_t *pos = (vga_pos_t *)obp->on_buf;
            vga_cursor_row = pos->row;
            vga_cursor_col = pos->col;
            if (vga_cursor_row >= VGA_ROWS) vga_cursor_row = VGA_ROWS - 1;
            if (vga_cursor_col >= VGA_COLS) vga_cursor_col = VGA_COLS - 1;
            vga_update_cursor();
        }
        break;
    case VGA_IOCTL_GET_CURSOR:
        if (obp->on_buf && obp->on_len >= sizeof(vga_pos_t)) {
            vga_pos_t *pos = (vga_pos_t *)obp->on_buf;
            pos->row = vga_cursor_row;
            pos->col = vga_cursor_col;
        }
        break;
    case VGA_IOCTL_SET_COLOR:
        if (obp->on_buf && obp->on_len >= 2) {
            u8_t *colors = (u8_t *)obp->on_buf;
            vga_color = vga_make_color(colors[0], colors[1]);
        }
        break;
    case VGA_IOCTL_SCROLL_UP:
        /* 向上滚动 */
        for (uint_t i = 0; i < (VGA_ROWS - 1) * VGA_COLS; i++) {
            vga_buffer[i] = vga_buffer[i + VGA_COLS];
        }
        {
            u16_t blank = vga_make_entry(' ', vga_color);
            for (uint_t i = (VGA_ROWS - 1) * VGA_COLS; i < VGA_SIZE; i++) {
                vga_buffer[i] = blank;
            }
        }
        break;
    case VGA_IOCTL_SCROLL_DOWN:
        /* 向下滚动 */
        for (uint_t i = VGA_SIZE - 1; i >= VGA_COLS; i--) {
            vga_buffer[i] = vga_buffer[i - VGA_COLS];
        }
        {
            u16_t blank = vga_make_entry(' ', vga_color);
            for (uint_t i = 0; i < VGA_COLS; i++) {
                vga_buffer[i] = blank;
            }
        }
        break;
    case VGA_IOCTL_SET_TEXTMODE:
        /* 切换到 VGA 文本模式 */
        if (!vga_text_mode) {
            vga_set_text_mode();
            vga_clear();
        }
        break;
    default:
        return DFCERRSTUS;
    }
    
    return DFCOKSTUS;
}

/**
 * @brief 启动设备
 */
drvstus_t vga_dev_start(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 停止设备
 */
drvstus_t vga_dev_stop(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 设置电源状态
 */
drvstus_t vga_set_powerstus(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 枚举设备
 */
drvstus_t vga_enum_dev(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

/**
 * @brief 刷新缓存
 */
drvstus_t vga_flush(device_t *devp, void *iopack) {
    vga_update_cursor();
    return DFCOKSTUS;
}

/**
 * @brief 关闭设备电源
 */
drvstus_t vga_shutdown(device_t *devp, void *iopack) {
    return DFCOKSTUS;
}

