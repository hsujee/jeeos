/**
 * buidcmd自动化编译配置文件 Makefile
 */

#include "config.h"
#ifdef CFG_X86_PLATFORM
ASM 	= nasm
CC		= gcc
LD		= ld
DD		= dd
RM		= rm
OYFLAGS		= -O binary
CFLAGS      = $(HEADFILE_PATH) -c -O2 -g -m64 -mtune=generic -mcmodel=large -mno-red-zone -mgeneral-regs-only -std=c99 -fexec-charset=UTF-8 -Wall -Wshadow  -W -Wconversion -Wno-sign-conversion -fno-stack-protector -fno-zero-initialized-in-bss -fomit-frame-pointer -fno-builtin -fno-common  -fno-ident -ffreestanding  -Wno-unused-parameter -Wunused-variable
LDFLAGS		= -static -T jeelink.lds -n -Map jeeos.map
CPPFLGSLDS	= $(HEADFILE_PATH) -E -P
ASMFLGS		= $(HEADFILE_PATH)  -f elf64
OBJCOPY = objcopy
OJCYFLAGS = -S -O binary
#endif

