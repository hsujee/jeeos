/*
 * @Author: Jee Hsu
 * @Description: 内核主函数入口
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "console.h"

void main(void) {
    // 先清屏，确保干净的显示环境
    clear_screen();
    
    // 显示系统信息
    printf("=====================================\n");
    printf("        Welcome to Jee OS\n");
    printf("=====================================\n");
    printf("Version : 0.01\n");
    printf("Author  : Jee Hsu\n");
    printf("=====================================\n");
    printf("\n");
    printf("System initialized successfully!\n");
    
    // 内核主循环 - 永不退出
    while (1) {
        __asm__ volatile ("hlt");  // 进入低功耗等待状态
    }
}
