MAKEFLAGS =-sR
KERNELCE_PATH	= ../arch/intelx86/kernel/
HEADFILE_PATH = ../arch/intelx86/include/
KRNLBOOT_PATH = ../arch/intelx86/boot/
CCBUILDPATH	= $(KRNLBOOT_PATH)
include buildcmd.mh
include objs.mh

.PHONY : all everything build_kernel
all: build_kernel 

build_kernel:everything

everything :  $(BUILD_MK_IMG_EBIN)
 
$(BUILD_MK_IMG_EBIN): $(BUILD_MK_IMG_LINK)  
	$(CC) -o $@ -static $(BUILD_MK_IMG_LINK)
