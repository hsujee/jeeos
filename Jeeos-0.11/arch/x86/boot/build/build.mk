.PHONY : all build
all: build

build: bootsect.o bootsect_c.o vgastr.o setup.o setup_c.o fs.o cpumem.o graph.o bootparam_c.o bootparam.o

CCSTR		= 	'CC -[M] Building '$<
PRINTCSTR 	=	@echo $(CCSTR) 

%.o : ../%.asm
	nasm -I ../include/ -f elf -o $@ $<
	$(PRINTCSTR)
%.o : ../%.c
	gcc -I ../include/ -c -Os -std=c99 -m32 -Wall -Wshadow -W -Wconversion -Wno-sign-conversion  -fno-stack-protector -fomit-frame-pointer -fno-builtin -fno-common  -fno-ident -ffreestanding  -fno-stack-protector -fomit-frame-pointer -Wno-unused-parameter -Wunused-variable -o $@ $<
	$(PRINTCSTR)
%.lds : ../%.S
	gcc ../include/ -E -P -o $@ $<
	$(PRINTCSTR)
%.mh : ../%.S
	gcc ../include/ -E -P -o $@ $<
	$(PRINTCSTR)
%.lib : ../%.c
	gcc -I ../include/ -c -O2 -m64  -mcmodel=large -mno-red-zone -std=c99 -Wall -Wshadow -W -Wconversion -Wno-sign-conversion -fno-stack-protector  -fomit-frame-pointer -fno-builtin -fno-common  -fno-ident -ffreestanding  -Wno-unused-parameter -Wunused-variable #-fdata-sections -gstabs+ -o $@ $<
	$(PRINTCSTR)
%.lib : ../%.asm
	nasm -I ../include/ -f elf64  #-mregparm=0-finline-functions-mcmodel=medium -mcmodel=large  -o $@ $<
	$(PRINTCSTR)