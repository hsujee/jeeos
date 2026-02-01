/*
 * @Author: Jee Hsu
 * @Description: 用户态格式化打印(printf)
 *
 * 实现标准C库的printf函数：
 * - 格式化字符串解析
 * - 类型转换(整数/字符串/十六进制)
 * - 通过系统调用write输出
 *
 * 支持的格式说明符：
 * - %d: 十进制整数
 * - %x: 十六进制整数
 * - %s: 字符串
 * - %c: 字符
 *
 * 依赖：
 * - mallocblk(): 临时缓冲区分配
 * - write(): 系统调用输出
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "stdio.h"
#include "string.h"

int printf(const char* fmt,...) {
    int rets=-1;
    va_list ap; 
    va_start(ap,fmt);
	char* buf=(char*)mallocblk(0x1000);
	if(buf==NULL) {
        return -1;
	}
    devid_t dev;
    dev.dev_mtype=UART_DEVICE;
    dev.dev_stype=0;
    dev.dev_nr=0;
    hand_t fd=open(&dev,RW_FLG|FILE_TY_DEV,0);
    if(fd==-1) {
        rets=-1;
        goto res_step;
    }
	vsprintf(buf,fmt,ap);
	if(write(fd,buf,strlen(buf),0)==SYSSTUSERR) {
        rets=-1;
        goto res_step;   
	}
	close(fd);
	rets=0;
res_step:
	if(mfreeblk(buf,0x1000)==SYSSTUSERR) {
        rets=-1;
	}
	
    va_end(ap);
	return rets;
}

char* strcopy(char* buf,char* str_s) {
	while(*str_s) {
		*buf=*str_s;
		buf++;
		str_s++;
	}
	return buf;
}

void vsprintf(char* buf,const char* fmt,va_list args) {
	char* p =buf;
	
	while(*fmt) {
		if(*fmt != '%') {
			*p++ = *fmt++;
			continue;
		}
		fmt++;
		switch(*fmt) {
			case 'x':
				p=number(p,va_arg(args,uint_t),16);
				fmt++;
				break;
			case 'd':
				p=number(p,va_arg(args,uint_t),10);
				fmt++;
				break;
		        case 's':
			       p=strcopy(p,(char*)va_arg(args,uint_t));
			       fmt++;
			       break;
			default:
				break;
		}
	}
	*p=0;
	return;
}

char* number(char* str,uint_t n, sint_t base) {
	register char *p;
	char strbuf[36];
    p = &strbuf[36];
    *--p = 0;  
	if(n == 0) {
        *--p = '0';
    } else {
        do {
            *--p = "0123456789abcdef"[n % base]; 
        }while( n /= base);
    }
    while(*p!=0) {
		*str++=*p++;
	}
    return str;
}