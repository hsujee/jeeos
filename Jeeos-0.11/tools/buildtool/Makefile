MAKEFLAGS =-sR

MKDIR = mkdir
RMDIR = rmdir
CP = cp
CD = cd
DD = dd
RM = rm

MAKE = make
PREBUILDMFLGS = -C $(BUILD_PATH) -f prebuild.mk
BUILDMFLGS = -C $(BUILD_PATH) -f build.mk
LINKMFLGS = -C $(BUILD_PATH) -f link.mk
CPFLAGES = 
DDFLAGS = 

BUILD_PATH = ./build
EXKNL_PATH = ./exckrnl
VM_PATH = ./vm
DSTPATH = ../exckrnl
RELEDSTPATH = ../release

.PHONY : print clean all

build: clean print all

all:
	$(MAKE) $(PREBUILDMFLGS)
	$(MAKE) $(BUILDMFLGS)
	$(MAKE) $(LINKMFLGS)
	@echo 'Build system is successfull......'

clean:
	$(CD) $(BUILD_PATH); $(RM) -f *.o *.bin *.i *.krnl *.s *.map *.lib *.btoj *.vdi *vmdk krnlobjs.mh

print:
	@echo '*********Starting compile build system*************'




