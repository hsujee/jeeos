MAKEFLAGS = -s
KERNELCE_PATH	= ../script/
HEADFILE_PATH = -I ../include/
CCBUILDPATH	= $(KERNELCE_PATH)
include buildcmd.mh

PREMENT_OBJS = objs.mh
.PHONY : all everything  build_kernel
all: build_kernel

build_kernel:everything

everything : $(PREMENT_OBJS)
include buildrule.mh
