/*
 * @Author: Jee Hsu
 * @Description: imgerror模块头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _IMGERROR_H
#define _IMGERROR_H
#define img_error(x)                                                                    \
    do {                                                                                    \
        printf("LMKIMG ERR:%s ", x);                                                    \
        printf("C file:%s C line:%d C function:%s\n", __FILE__, __LINE__, __FUNCTION__); \
        exit(0);                                                                         \
    } while (0);
void init_error();
void exit_error();
#endif
