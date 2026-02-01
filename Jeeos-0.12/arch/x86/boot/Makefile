.PHONY : build clean all

build: clean all

all:
	@echo "Start build boot ..."
	make -C ./build -f build.mk
	make -C ./build -f link.mk
	@echo "End build boot ..."
clean:
	cd ./build; rm -f *.o *.bin *.i *.krnl *.s *.map *.lib *.btoj *.vdi *vmdk *.eki *.elf