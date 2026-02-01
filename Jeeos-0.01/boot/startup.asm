;
; @Author: Jee Hsu
; @Description: 引导启动程序
;
; Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
;
MBT_HDR_FLAGS    EQU 0x00010003
; 定义 GRUB1 多引导协议魔数
MBT_HDR_MAGIC    EQU 0x1BADB002
; 定义 GRUB2 多引导协议魔数
MBT_HDR2_MAGIC   EQU 0xe85250d6

; 导出 _start 符号作为程序入口点
global _start
; 声明外部 main 函数，将由 C 代码实现
extern main

; 定义代码段 (必须添加 exec 属性以确保节标记为可执行)
[section .start.text exec alloc]
; 指示汇编器生成 32 位代码
[bits 32]

; 程序入口点
_start:
    ; 跳转到实际入口代码
    jmp _entry

; 8 字节对齐 (GRUB1 规范要求)
ALIGN 8
; GRUB1 多引导头开始
mbt_hdr:
    ; 魔数 (必须为 0x1BADB002)
    dd MBT_HDR_MAGIC
    ; 标志位
    dd MBT_HDR_FLAGS
    ; 校验和 (魔数 + 标志 + 校验和 = 0)
    dd -(MBT_HDR_MAGIC+MBT_HDR_FLAGS)
    ; 多引导头地址
    dd mbt_hdr
    ; 代码段起始地址
    dd _start
    ; 未使用的字段 (应为 0)
    dd 0
    dd 0
    ; 入口地址
    dd _entry
; GRUB1 多引导头结束

; 8 字节对齐 (GRUB2 规范要求)
ALIGN 8
; GRUB2 多引导头开始
mbt2_hdr:
    ; 魔数 (必须为 0xe85250d6)
    dd MBT_HDR2_MAGIC
    ; 架构 (0 表示 32 位保护模式)
    dd 0
    ; 头长度
    dd mbt2_hdr_end - mbt2_hdr
    ; 校验和
    dd -(MBT_HDR2_MAGIC + 0 + (mbt2_hdr_end - mbt2_hdr))
    ; 信息请求标签 (类型 2)
    dw 2, 0
    ; 标签大小 (24 字节)
    dd 24
    ; 头地址
    dd mbt2_hdr
    ; 加载地址
    dd _start
    ; 未使用的字段
    dd 0
    dd 0
    ; 入口地址标签 (类型 3)
    dw 3, 0
    ; 标签大小 (12 字节)
    dd 12
    ; 入口地址
    dd _entry
    ; 未使用的字段
    dd 0
    ; 结束标签 (类型 0)
    dw 0, 0
    ; 标签大小 (8 字节)
    dd 8
mbt2_hdr_end:
; GRUB2 多引导头结束

; 8 字节对齐
ALIGN 8

; 实际入口点
_entry:
    ; 禁用中断
    cli
    ; 禁用不可屏蔽中断 (NMI)
    in al, 0x70     ; 读取 CMOS 索引寄存器
    or al, 0x80     ; 设置最高位 (禁用 NMI)
    out 0x70, al    ; 写回 CMOS 索引寄存器
    ; 加载全局描述符表 (GDT)
    lgdt [GDT_PTR]
    ; 远跳转以刷新 CS 寄存器并进入 32 位模式
    jmp dword 0x8:_32bits_mode

; 32 位保护模式代码
_32bits_mode:
    ; 设置数据段寄存器 (指向 GDT 中的数据段选择子)
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ; 清零通用寄存器
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    xor edi, edi
    xor esi, esi
    xor ebp, ebp
    ; 设置栈指针 (栈向下增长)
    mov esp, 0x9000
    ; 调用 C 主函数
    call main

; 停机循环
halt_step:
    hlt     ; 暂停 CPU 执行
    jmp halt_step  ; 无限循环

; 全局描述符表 (GDT) 开始
GDT_START:
; 空描述符 (必须为 0)
knull_dsc: dq 0
; 内核代码段描述符
; 基址=0, 界限=0xFFFFF, 粒度=4KB, 32 位代码段, 特权级 0
kcode_dsc: dq 0x00cf9e000000ffff
; 内核数据段描述符
; 基址=0, 界限=0xFFFFF, 粒度=4KB, 可写数据段, 特权级 0
kdata_dsc: dq 0x00cf92000000ffff
; 16 位代码段描述符 (保留)
k16cd_dsc: dq 0x00009e000000ffff
; 16 位数据段描述符 (保留)
k16da_dsc: dq 0x000092000000ffff
GDT_END:

; GDT 指针结构
GDT_PTR:
    ; GDT 长度 (减 1 是 x86 架构要求)
    dw GDT_END-GDT_START-1
    ; GDT 基地址
    dd GDT_START
