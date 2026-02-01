/**
 * system自动化编译配置文件
 */

MAKEFLAGS = -s
SYSTEMCE_PATH	= ../system/
HEADFILE_PATH = -I ../\
                -I ../system/include\
                -I ../kernel/include\
                -I ../libs/include/types\
                -I ../libs/include\
                -I ../arch/x86/include\
                -I ../system/include
CCBUILDPATH	= $(SYSTEMCE_PATH)
include buidcmd.mk
include objs.mk

.PHONY : all everything  build_system
all: build_system

build_system:everything

everything :$(BUILD_MK_SYST_OBJS)
include buidrule.mk
