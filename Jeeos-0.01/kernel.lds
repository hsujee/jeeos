/*
 * Jeeos-0.01 链接脚本
 *
 * 功能: 控制内核镜像的内存布局
 * 
 * 内存布局:
 *   0x200000 (2MB)  - 内核加载地址
 *   .start.text     - 启动代码段 (包含Multiboot头和入口点)
 *   .text           - 代码段
 *   .data           - 已初始化数据段
 *   .rodata         - 只读数据段
 *   .kstrtab        - 内核字符串表
 *   .bss            - 未初始化数据段
 *
 * 符号说明:
 *   __begin_xxx     - 各段起始地址
 *   __end_xxx       - 各段结束地址
 */

/* 入口点: startup.asm中的_start标签 */
ENTRY(_start)

/* 输出格式: 32位x86 ELF */
OUTPUT_ARCH(i386)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
	/* 内核加载到2MB位置，避开低端内存的BIOS区域 */
	. = 0x200000;

	/* 启动代码段 - 包含Multiboot头部和初始化代码 */
	__begin_start_text = .;
	.start.text : ALIGN(4) { *(.start.text) }
	__end_start_text = .;

	/* 代码段 - 主要的可执行代码 */
	__begin_text = .;
	.text : ALIGN(4) { *(.text) }
	__end_text = .;

	/* 数据段 - 已初始化的全局变量 */
	__begin_data = .;
	.data : ALIGN(4) { *(.data) }
	__end_data = .;

	/* 只读数据段 - 常量、字符串字面量 */
	__begin_rodata = .;
	.rodata : ALIGN(4) { *(.rodata) *(.rodata.*) }
	__end_rodata = .;

	/* 内核字符串表 - 调试信息 */
	__begin_kstrtab = .;
	.kstrtab : ALIGN(4) { *(.kstrtab) }
	__end_kstrtab = .;

	/* BSS段 - 未初始化的全局变量，运行时清零 */
	__begin_bss = .;
	.bss : ALIGN(4) { *(.bss) }
	__end_bss = .;
}
