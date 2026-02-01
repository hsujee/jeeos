/**
 * kernel自动化编译配置文件
 */

MAKEFLAGS = -s
KERNELCE_PATH	= ../kernel/
HEADFILE_PATH = -I ../\
                -I ../kernel/include\
                -I ../libs/include/types\
                -I ../libs/include\
                -I ../arch/x86/include\
                -I ../system/include
CCBUILDPATH	= $(KERNELCE_PATH)
include buidcmd.mk
include objs.mk

.PHONY : all everything  build_kernel
all: build_kernel

build_kernel:everything

everything :$(BUILD_MK_KRNL_OBJS)
include buidrule.mk