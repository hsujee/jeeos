/*
 * @Author: Jee Hsu
 * @Description: VGA文本模式控制台驱动
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "console.h"

#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define GREEN_ON_BLACK 0x02

// VGA CRT 控制寄存器端口
#define VGA_CTRL_REGISTER 0x3D4
#define VGA_DATA_REGISTER 0x3D5

// 全局光标状态
struct {
    int row;
    int col;
} cursor = {0, 0};

// 端口输出函数
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// 更新硬件光标位置
void update_cursor(void) {
    uint16_t pos = (uint16_t)(cursor.row * SCREEN_WIDTH + cursor.col);
    
    // 设置光标位置低8位
    outb(VGA_CTRL_REGISTER, 0x0F);
    outb(VGA_DATA_REGISTER, (uint8_t)(pos & 0xFF));
    
    // 设置光标位置高8位
    outb(VGA_CTRL_REGISTER, 0x0E);
    outb(VGA_DATA_REGISTER, (uint8_t)((pos >> 8) & 0xFF));
}

// 清屏函数
void clear_screen(void) {
    char* video_ptr = (char*)VIDEO_MEMORY;
    
    // 用空格填充整个屏幕
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 2; i += 2) {
        video_ptr[i] = ' ';           // 字符
        video_ptr[i + 1] = GREEN_ON_BLACK;  // 属性
    }
    
    // 重置光标位置
    cursor.row = 0;
    cursor.col = 0;
    update_cursor();
}

// 滚屏函数（将屏幕内容向上滚动一行）
static void scroll_screen(void) {
    char* video_ptr = (char*)VIDEO_MEMORY;
    
    // 将第2行到最后一行的内容复制到第1行到倒数第二行
    for (int i = 0; i < (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2; i++) {
        video_ptr[i] = video_ptr[i + SCREEN_WIDTH * 2];
    }
    
    // 清空最后一行
    int last_line_start = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2;
    for (int i = 0; i < SCREEN_WIDTH * 2; i += 2) {
        video_ptr[last_line_start + i] = ' ';
        video_ptr[last_line_start + i + 1] = GREEN_ON_BLACK;
    }
    
    // 光标回到最后一行开头
    cursor.row = SCREEN_HEIGHT - 1;
    cursor.col = 0;
}

void _strwrite(const char* string) {
    char* video_ptr = (char*)VIDEO_MEMORY;
    
    while (*string) {
        // 如果超出屏幕底部，滚屏
        if (cursor.row >= SCREEN_HEIGHT) {
            scroll_screen();
        }
        
        if (*string == '\n') {
            // 换行：移到下一行首
            cursor.row++;
            cursor.col = 0;
            string++;
            continue;
        }
        
        // 计算当前光标物理位置
        int pos = (cursor.row * SCREEN_WIDTH + cursor.col) * 2;
        video_ptr[pos] = *string++;        // 字符
        video_ptr[pos + 1] = GREEN_ON_BLACK; // 属性
        
        // 移动光标到下一个位置
        cursor.col++;
        
        // 行尾自动换行
        if (cursor.col >= SCREEN_WIDTH) {
            cursor.row++;
            cursor.col = 0;
        }
    }
    
    // 更新硬件光标
    update_cursor();
}

void printf(const char* fmt) {
    _strwrite(fmt);
}

