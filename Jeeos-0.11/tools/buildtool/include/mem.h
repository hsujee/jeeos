/*
 * @Author: Jee Hsu
 * @Description: mem模块头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _MEM_H
#define _MEM_H
#define MFLG_ALLOC 1
#define MFLG_FREE 2
void init_mem();
void exit_mem();
void* img_mem(long msize,void* mfradr,uint_t mflg);
void img_memcpy(void*dest,const void *src,size_t n);
void img_memclr(void* p,int c,size_t n);
#endif
