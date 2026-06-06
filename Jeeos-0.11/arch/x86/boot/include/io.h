/*
 * @Author: Jee Hsu
 * @Description: 引导模块 - io
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _IO_H
#define _IO_H
KLINE void out_u8(const u16_t port, const u8_t val) {
    __asm__ __volatile__("outb  %1, %0\n"
             :
             : "dN"(port), "a"(val));
}

KLINE u8_t in_u8(const u16_t port) {
    u8_t tmp;
    __asm__ __volatile__("inb %1, %0\n"
             : "=a"(tmp)
             : "dN"(port));
    return tmp;
}

KLINE void out_u16(const u16_t port, const u16_t val) {
    __asm__ __volatile__("outw  %1, %0\n"
             :
             : "dN"(port), "a"(val));
}

KLINE u16_t in_u16(const u16_t port) {
    u16_t tmp;

    __asm__ __volatile__("inw %1, %0\n"
             : "=a"(tmp)
             : "dN"(port));
    return tmp;
};

KLINE sint_t m2mcopy(void* sadr,void* dadr,sint_t len) {
    if(NULL==sadr||NULL==dadr||1>len) {
        return 0;
    }
    u8_t* s=(u8_t*)sadr,*d=(u8_t*)dadr;
    if(s<d) {
        for(sint_t i=(len-1);i>=0;i--) {
            d[i]=s[i];
        }
        return len;
    }
    if(s>d) {
        for(sint_t j=0;j<len;j++) {
            d[j]=s[j];
        }
        return len;
    }
    if(s==d) {
        return len;
    }
    return 0;
}

KLINE void memset(void* src,u8_t val,uint_t count) {
    u8_t* ss=src;
    for(uint_t i=0;i<count;i++) {
        ss[i]=val;
    }
    return;
}

// PCI 配置空间访问
#define PCI_CONFIG_ADDRESS  0xCF8
#define PCI_CONFIG_DATA     0xCFC

KLINE void out_u32(const u16_t port, const u32_t val) {
    __asm__ __volatile__("outl  %1, %0\n"
             :
             : "dN"(port), "a"(val));
}

KLINE u32_t in_u32(const u16_t port) {
    u32_t tmp;
    __asm__ __volatile__("inl %1, %0\n"
             : "=a"(tmp)
             : "dN"(port));
    return tmp;
}

// 读取 PCI 配置空间寄存器
KLINE u32_t pci_read_config(u8_t bus, u8_t device, u8_t func, u8_t offset) {
    u32_t address = (u32_t)((1 << 31) |           // Enable bit
                           ((u32_t)bus << 16) |    // Bus number
                           ((u32_t)device << 11) | // Device number
                           ((u32_t)func << 8) |    // Function number
                           (offset & 0xFC));       // Register offset (aligned to 4 bytes)
    out_u32(PCI_CONFIG_ADDRESS, address);
    return in_u32(PCI_CONFIG_DATA);
}

#endif
