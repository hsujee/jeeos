.PHONY : all elf build bin
all: build 
#INITLDR
build:elf bin
elf : bootsect.elf setup.elf bootparam.elf
bin:bootsect.bin setup.bin bootparam.bin

CCSTR		= 	'CC -[M] Building '$<
PRINTCSTR 	=	@echo $(CCSTR) 

bootsect.elf: bootsect.o bootsect_c.o vgastr.o
	ld -s -T bootsect.lds -n  -Map bootsect.map -o $@ bootsect.o bootsect_c.o vgastr.o
setup.elf: setup.o setup_c.o fs.o cpumem.o graph.o bootparam_c.o vgastr.o
	ld -s -T setup.lds -n  -Map setup.map -o $@ setup.o setup_c.o fs.o cpumem.o graph.o bootparam_c.o vgastr.o
bootparam.elf: bootparam.o
	ld -s -T bootparam.lds -n  -Map bootparam.map -o $@ bootparam.o
bootsect.bin:bootsect.elf
	objcopy -S -O binary $< $@
	$(PRINTCSTR)
setup.bin:setup.elf
	objcopy -S -O binary $< $@
	$(PRINTCSTR)
bootparam.bin:bootparam.elf
	objcopy -S -O binary $< $@
	$(PRINTCSTR)