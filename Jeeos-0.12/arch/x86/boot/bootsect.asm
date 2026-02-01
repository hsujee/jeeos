;------------------------------------------------------------------------------
; @Author: Jee Hsu
; @Description: 引导扇区程序 - GRUB多引导规范入口
;
; 由GRUB引导加载，是系统启动的第一个代码：
; 1. 实现GRUB1/GRUB2多引导协议头
; 2. 初始化栈和基本运行环境
; 3. 调用C语言的head_entry()
;
; 多引导协议要求：
; - 必须在文件头8KB内包含多引导头
; - GRUB1魔数: 0x1BADB002
; - GRUB2魔数: 0xE85250D6
;
; 启动后内存布局：
; - 0x1000: bootparam.bin
; - 0x8000: setup.bin
; - 0x100000: jeeos.eki
;
; Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
;------------------------------------------------------------------------------
;想要使用 GRUB 引导系统启动，则需要实现 GRUB 头数据
;出于兼容性考虑，这里对 GRUB1 和 GRUB2 都做了实现
MBT_HDR_FLAGS	EQU 0x00010003
;GRUB1 多引导协议头魔数
MBT_HDR_MAGIC	EQU 0x1BADB002
;GRUB2 多引导协议头魔数
MBT2_HDR_MAGIC	EQU 0xE85250D6
;导出_start符号
global _start
;导入外部的head_entry函数符号
extern head_entry
;定义.text代码节
[section .text]
;汇编成32位代码
[bits 32]
_start:
	jmp _entry
ALIGN 4
mbthdr:
	DD MBT_HDR_MAGIC
	DD MBT_HDR_FLAGS
	DD -(MBT_HDR_MAGIC+MBT_HDR_FLAGS)
	DD mbthdr
	DD _start
	DD 0
	DD 0
	DD _entry
;以上是GRUB所需要的头
ALIGN 8
mbt2hdr:
	DD	MBT2_HDR_MAGIC
	DD	0
	DD	mbt2hdrend - mbt2hdr
	DD	-(MBT2_HDR_MAGIC + 0 + (mbt2hdrend - mbt2hdr))
	DW	2, 0
	DD	24
	DD	mbt2hdr
	DD	_start
	DD	0
	DD	0
	DW	3, 0
	DD	12
	DD	_entry 
	DD      0  
	DW	0, 0
	DD	8
mbt2hdrend:
;以上是GRUB2所需要的头
;包含两个头是为了同时兼容GRUB、GRUB2

_entry:
	;关中断
	cli
	;关掉不可屏蔽中断
	in al, 0x70
	or al, 0x80	
	out 0x70,al
	
	;========================================
	; 关闭 GRUB 可能启用的长模式和分页
	;========================================
	
	;1. 先关闭分页
	mov eax, cr0
	and eax, 0x7fffffff  ; 清除 CR0.PG (bit 31)
	mov cr0, eax
	
	;2. 关闭 PAE
	mov eax, cr4
	and eax, 0xffffffdf  ; 清除 CR4.PAE (bit 5)
	mov cr4, eax
	
	;3. 关闭长模式 (EFER.LME)
	mov ecx, 0xc0000080  ; IA32_EFER MSR
	rdmsr
	and eax, 0xfffffeff  ; 清除 EFER.LME (bit 8)
	wrmsr
	
	;========================================
	
	;重新加载GDT，加载GDT地址到GDTR寄存器
	lgdt [GDT_PTR]
	;长跳转刷新CS影子寄存器
	jmp dword 0x8 :_32bits_mode

_32bits_mode:
	;下面初始化C语言可能会用到的寄存器
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	xor eax,eax
	xor ebx,ebx
	xor ecx,ecx
	xor edx,edx
	xor edi,edi
	xor esi,esi
	xor ebp,ebp
	xor esp,esp
	;设置栈顶为0x7c00
	mov esp,0x7c00
	;调用head_entry函数在bootsect_c.c中实现
	call head_entry
	;head_entry函数会将setup.bin复制到内存0x200000的位置
	;jmp跳转到0x200000地址开始执行
	jmp 0x200000

GDT_START:
knull_dsc: dq 0
kcode_dsc: dq 0x00cf9e000000ffff
kdata_dsc: dq 0x00cf92000000ffff
k16cd_dsc: dq 0x00009e000000ffff ;16位代码段描述符
k16da_dsc: dq 0x000092000000ffff ;16位数据段描述符
GDT_END:
GDT_PTR:
;GDT界限
GDTLEN	dw GDT_END-GDT_START-1
GDTBASE	dd GDT_START
