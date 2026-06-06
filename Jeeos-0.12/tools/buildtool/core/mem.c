/*
 * @Author: Jee Hsu
 * @Description: mem模块
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "imgheads.h"

void init_mem() {
    return;
}

void exit_mem() {
    return;
}

void* img_mem(long msize,void* mfradr,uint_t mflg) {
    if(mflg==MFLG_ALLOC) {
        return malloc(msize);
    }
    if(mflg==MFLG_FREE) {
        free(mfradr);
        return (void*)1;
    }
    return NULL;
}

void img_memcpy(void*dest,const void *src,size_t n) {
    void* buf=memcpy(dest,src,n);
    if(buf==dest) {
        return;
    }
    return;
}

void img_memclr(void* p,int c,size_t n) {
    void *buf=memset(p,c,n);
    if(buf==NULL) {
        img_error("memclr");
    }
    return;
}