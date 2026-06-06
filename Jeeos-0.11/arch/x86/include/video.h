/*
 * @Author: Jee Hsu
 * @Description: 视频驱动 - VGA/VBE/BGA图形显示
 *
 * 提供图形显示功能，支持：
 * - VBE (VESA BIOS Extensions) 模式
 * - BGA (Bochs Graphics Adapter) 模式
 * - 像素绘制、字符显示、屏幕刷新
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_VIDEO_H
#define ARCH_X86_VIDEO_H

/*===========================================================================
 * 初始化函数
 *===========================================================================*/

/**
 * @brief 初始化默认图形设备
 */
void init_dftgraph();

/**
 * @brief 显示系统Logo
 */
void arch_logo();

/** @brief 设置红色 */
void red();
/** @brief 设置绿色 */
void gre();
/** @brief 设置蓝色 */
void blu();
/** @brief 设置黑色 */
void blk();
/** @brief 设置白色 */
void wri();

/**
 * @brief 设置默认字体颜色
 * @param setpx 像素颜色值 (使用 BGRA 宏)
 * @return 之前的字体颜色
 */
pixl_t set_deffontpx(pixl_t setpx);

/**
 * @brief 初始化视频子系统
 */
void init_archvideo();

/**
 * @brief 初始化BGA适配器
 */
void init_bga();

/**
 * @brief 初始化VBE模式
 */
void init_vbe();

/*===========================================================================
 * 图形操作函数
 *===========================================================================*/

/**
 * @brief 填充整个屏幕
 * @param kghp 图形设备指针
 * @param pix 填充颜色
 */
void fill_graph(dftgraph_t* kghp, pixl_t pix);

/**
 * @brief 写入像素到后备缓冲
 * @param kghp 图形设备指针
 * @param x X坐标
 * @param y Y坐标
 * @param pix 像素颜色
 */
void write_pixcolor(dftgraph_t* kghp, u32_t x, u32_t y, pixl_t pix);

/**
 * @brief 直接写入像素到显存
 * @param kghp 图形设备指针
 * @param x X坐标
 * @param y Y坐标
 * @param pix 像素颜色
 */
void drxw_pixcolor(dftgraph_t* kghp, u32_t x, u32_t y, pixl_t pix);

/**
 * @brief 刷新显存
 * @param kghp 图形设备指针
 */
void flush_videoram(dftgraph_t* kghp);

/*===========================================================================
 * 字符显示函数
 *===========================================================================*/

/**
 * @brief UTF-8转Unicode
 * @param utfp UTF-8字符指针
 * @param retuib 返回UTF-8字节数
 * @return Unicode码点
 */
u32_t utf8_to_unicode(utf8_t* utfp, int* retuib);

/**
 * @brief 获取字符字形信息
 * @param kghp 图形设备指针
 * @param unicode Unicode码点
 * @param retchwx 返回字符宽度
 * @param retchhx 返回字符高度
 * @param retlinb 返回行字节数
 * @return 字形数据指针
 */
u8_t* ret_charsinfo(dftgraph_t* kghp, u32_t unicode, int* retchwx, int* retchhx, int* retlinb);

/**
 * @brief 绘制单个字符到后备缓冲
 * @param kghp 图形设备指针
 * @param unicode Unicode码点
 * @param pix 字符颜色
 */
void put_pixonechar(dftgraph_t* kghp, u32_t unicode, pixl_t pix);

/**
 * @brief 直接绘制单个字符到显存
 * @param kghp 图形设备指针
 * @param unicode Unicode码点
 * @param pix 字符颜色
 */
void drxput_pixonechar(dftgraph_t* kghp, u32_t unicode, pixl_t pix);

/**
 * @brief 输出字符串
 * @param kghp 图形设备指针
 * @param buf 字符串缓冲区
 */
void gstr_write(dftgraph_t* kghp, char_t* buf);

/*===========================================================================
 * BGA驱动接口
 *===========================================================================*/

/**
 * @brief BGA读取函数
 * @param ghpdev 图形设备指针
 * @param outp 输出缓冲区
 * @param rdsz 读取大小
 * @return 实际读取字节数
 */
size_t bga_read(void* ghpdev, void* outp, size_t rdsz);

/**
 * @brief BGA写入函数
 * @param ghpdev 图形设备指针
 * @param inp 输入缓冲区
 * @param wesz 写入大小
 * @return 实际写入字节数
 */
size_t bga_write(void* ghpdev, void* inp, size_t wesz);

/**
 * @brief BGA控制函数
 * @param ghpdev 图形设备指针
 * @param outp 输出参数
 * @param iocode 控制码
 * @return 0成功，负值失败
 */
sint_t bga_ioctrl(void* ghpdev, void* outp, uint_t iocode);

/** @brief BGA刷新显存 */
void bga_flush(void* ghpdev);

/** @brief BGA设置显存bank */
sint_t bga_set_bank(void* ghpdev, sint_t bnr);

/** @brief BGA读取像素 */
pixl_t bga_readpix(void* ghpdev, uint_t x, uint_t y);

/** @brief BGA写入像素 */
void bga_writepix(void* ghpdev, pixl_t pix, uint_t x, uint_t y);

/** @brief BGA直接读取像素 */
pixl_t bga_dxreadpix(void* ghpdev, uint_t x, uint_t y);

/** @brief BGA直接写入像素 */
void bga_dxwritepix(void* ghpdev, pixl_t pix, uint_t x, uint_t y);

/** @brief BGA设置坐标 */
sint_t bga_set_xy(void* ghpdev, uint_t x, uint_t y);

/** @brief BGA设置虚拟分辨率 */
sint_t bga_set_vwh(void* ghpdev, uint_t vwt, uint_t vhi);

/** @brief BGA设置偏移 */
sint_t bga_set_xyoffset(void* ghpdev, uint_t xoff, uint_t yoff);

/** @brief BGA获取坐标 */
sint_t bga_get_xy(void* ghpdev, uint_t* rx, uint_t* ry);

/** @brief BGA获取虚拟分辨率 */
sint_t bga_get_vwh(void* ghpdev, uint_t* rvwt, uint_t* rvhi);

/** @brief BGA获取偏移 */
sint_t bga_get_xyoffset(void* ghpdev, uint_t* rxoff, uint_t* ryoff);

/*===========================================================================
 * VBE驱动接口
 *===========================================================================*/

/** @brief VBE读取函数 */
size_t vbe_read(void* ghpdev, void* outp, size_t rdsz);

/** @brief VBE写入函数 */
size_t vbe_write(void* ghpdev, void* inp, size_t wesz);

/** @brief VBE控制函数 */
sint_t vbe_ioctrl(void* ghpdev, void* outp, uint_t iocode);

/** @brief VBE刷新显存 */
void vbe_flush(void* ghpdev);

/** @brief VBE设置显存bank */
sint_t vbe_set_bank(void* ghpdev, sint_t bnr);

/** @brief VBE读取像素 */
pixl_t vbe_readpix(void* ghpdev, uint_t x, uint_t y);

/** @brief VBE写入像素 */
void vbe_writepix(void* ghpdev, pixl_t pix, uint_t x, uint_t y);

/** @brief VBE直接读取像素 */
pixl_t vbe_dxreadpix(void* ghpdev, uint_t x, uint_t y);

/** @brief VBE直接写入像素 */
void vbe_dxwritepix(void* ghpdev, pixl_t pix, uint_t x, uint_t y);

/** @brief VBE设置坐标 */
sint_t vbe_set_xy(void* ghpdev, uint_t x, uint_t y);

/** @brief VBE设置虚拟分辨率 */
sint_t vbe_set_vwh(void* ghpdev, uint_t vwt, uint_t vhi);

/** @brief VBE设置偏移 */
sint_t vbe_set_xyoffset(void* ghpdev, uint_t xoff, uint_t yoff);

/** @brief VBE获取坐标 */
sint_t vbe_get_xy(void* ghpdev, uint_t* rx, uint_t* ry);

/** @brief VBE获取虚拟分辨率 */
sint_t vbe_get_vwh(void* ghpdev, uint_t* rvwt, uint_t* rvhi);

/** @brief VBE获取偏移 */
sint_t vbe_get_xyoffset(void* ghpdev, uint_t* rxoff, uint_t* ryoff);

#endif // ARCH_X86_VIDEO_H
