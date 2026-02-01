/**
 * link自动化编译配置文件 Makefile
 */

MAKEFLAGS =

include buidcmd.mk
include objs.mk

.PHONY : all everything build_kernel
all: build_kernel

build_kernel:everything
#$(LDER_EXC_BIN)

everything :   
[$(LD) $(LDFLAGS) -o $(KERNL_MK_ELFF_FILE) $(BUILD_MK_LINK_OBJS)
[@echo 'LD -[M] Linking ' $@
[$(OBJCOPY) $(OJCYFLAGS) $(KERNL_MK_ELFF_FILE) $(KERNL_MK_BINF_FILE)

#$(BOOT_EXC_ELF) $(BOOT_EXC_BIN)

$(BOOT_EXC_ELF): $(JEEOSEM_LINK_OBJS)
[$(LD) $(LDFLAGS) -o $(BOOT_EXC_ELF)  $(JEEOSEM_LINK_OBJS)
[@echo 'LD -[M] Building ' $@
$(BOOT_EXC_BIN):

//$(BUILD_MK_LMKR_EBIN): $(BUILD_MK_LMKR_LINK)
//	$(LD) $(KERNELLDFLAGS) -o $(BUILD_MK_LMKR_EBIN) $(BUILD_MK_LMKR_LINK)