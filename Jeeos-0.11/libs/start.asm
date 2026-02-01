;------------------------------------------------------------------------------
; @Author: Jee Hsu
; @Description: 用户程序入口点(_start)
;
; 用户程序的真正入口点：
; 1. 由内核加载用户程序后跳转到此
; 2. 调用C语言的main()函数
; 3. main()返回后死循环(应调用exit)
;
; 链接时_start必须作为入口：
; ld -e _start ...
;
; Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
;------------------------------------------------------------------------------
extern main

global _start

[section .text]
[BITS 64]
_start:
	xor rax,rax
	mov rax,main
	call rax
	jmp $

