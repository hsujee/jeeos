/**
 * kernel自动化编译配置文件
 */

MAKEFLAGS = -s
KERNELCE_PATH	= ../system/
HEADFILE_PATH = -I ../\
                -I ../system/include\
                -I ../kernel/include\
                -I ../libs/include/types\
                -I ../libs/include\
                -I ../arch/x86/include\
                -I ../drivers/include
CCBUILDPATH	= $(KERNELCE_PATH)
include buidcmd.mk
include objs.mk

.PHONY : all everything  build_kernel
all: build_kernel

build_kernel:everything

everything :$(BUILD_MK_KSYS_OBJS)
include buidrule.mk