/**
 * arch自动化编译配置文件 Makefile
 */

#include "config.h"
MAKEFLAGS = -s
#ifdef CFG_X86_PLATFORM
KERNELCE_PATH	= ../arch/x86/kernel/
HEADFILE_PATH = -I ../arch/x86/include\
                -I ../\
                -I ../libs/include/types\
                -I ../libs/include\
                -I ../system/include\
                -I ../kernel/include\
                -I ../drivers/include
#endif
#ifdef CFG_S3C2440A_PLATFORM
KERNELCE_PATH	= ../arch/s3c2440/
HEADFILE_PATH = -I ../include -I ../include/types -I ../include/archinc -I ../include/knlinc -I ../include/libinc -I ../include/drvinc
#endif
#ifdef CFG_STM32F0XX_PLATFORM
KERNELCE_PATH	= ../arch/stm32f0xx/
HEADFILE_PATH = -I ../include -I ../include/types -I ../include/archinc -I ../include/knlinc -I ../include/libinc -I ../include/drvinc
#endif

CCBUILDPATH	= $(KERNELCE_PATH)
include buidcmd.mk
include objs.mk

.PHONY : all everything  build_kernel
all: build_kernel

build_kernel:everything

everything :$(BUILD_MK_ARCH_OBJS)
include buidrule.mk
