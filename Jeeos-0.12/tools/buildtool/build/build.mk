MAKEFLAGS = -s
KERNELCE_PATH	= ../core/
HEADFILE_PATH = ../include/

CCBUILDPATH	= $(KERNELCE_PATH)
include buildcmd.mh
include objs.mh

.PHONY : all everything  build_kernel
all: build_kernel 

build_kernel:everything
	
everything : $(BUILD_MK_CORE_OBJS) 

include buildrule.mh
