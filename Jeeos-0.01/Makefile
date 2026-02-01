###########################################################################
# Jeeos-0.01 主构建文件
#
# 功能: 编译微型内核、生成镜像、运行虚拟机
#
# 项目结构:
#   boot/       - 引导代码 (startup.asm)
#   kernel/     - 内核代码 (main.c)
#   lib/        - 库函数 (console.c/h)
#   build/      - 编译输出目录
#   tools/      - 工具脚本
#
# 构建流程:
#   1. 编译 startup.asm -> startup.o (NASM)
#   2. 编译 main.c -> main.o (GCC)
#   3. 编译 console.c -> console.o (GCC)
#   4. 链接生成 JeeOS.elf (LD)
#   5. 转换为 JeeOS.bin (objcopy)
#
# 主要目标:
#   build     - 编译内核
#   install   - 生成二进制镜像
#   qemurun   - QEMU运行
#   qemudebug - QEMU调试模式 (GDB端口1234)
#   vboxrun   - VirtualBox运行
###########################################################################

MAKEFLAGS = -sR

#---------------------------------------------------------------------------
# 目录定义
#---------------------------------------------------------------------------
# 引导代码目录
BOOT_DIR    := boot
# 内核代码目录
KERNEL_DIR  := kernel
# 库函数目录
LIB_DIR     := lib
# 脚本目录
SCRIPTS_DIR := scripts
# 编译输出目录
BUILD_DIR   := build
# 工具目录
TOOLS_DIR   := tools

#---------------------------------------------------------------------------
# 目标文件列表
#---------------------------------------------------------------------------
JEEOS_OBJS := $(BUILD_DIR)/startup.o $(BUILD_DIR)/main.o $(BUILD_DIR)/console.o

#---------------------------------------------------------------------------
# 编译选项
#---------------------------------------------------------------------------
# GCC编译选项:
#   -c              仅编译不链接
#   -Os             优化大小
#   -std=c99        使用C99标准
#   -m32            生成32位代码
#   -Wall           开启所有警告
#   -fno-builtin    不使用内置函数
#   -ffreestanding  独立环境(无标准库)
#   -fno-pic        禁用位置无关代码
#   -g              生成调试信息
CFLAGS := -c -Os -std=c99 -m32 -Wall -Wshadow -W -Wconversion \
          -Wno-sign-conversion -fno-stack-protector -fomit-frame-pointer \
          -fno-builtin -fno-common -ffreestanding -Wno-unused-parameter \
          -Wunused-variable -fno-pic -fno-pie -I$(LIB_DIR) -g

# NASM汇编选项:
#   -f elf32        输出32位ELF格式
#   -g              生成调试信息
NASMFLAGS := -f elf32 -w-orphan-labels -g

# 链接选项:
#   -static         静态链接
#   -T kernel.lds   使用链接脚本
#   -Map            生成映射文件
#   -m elf_i386     32位ELF输出
LDFLAGS := -static -T kernel.lds -n -Map $(BUILD_DIR)/JeeOS.map -m elf_i386

#---------------------------------------------------------------------------
# QEMU虚拟机配置
#---------------------------------------------------------------------------
QEMU = qemu-system-i386
QEMU_OPTS = -m 64 -serial stdio              # 64MB内存，串口重定向到标准输出
QEMU_DEBUG_OPTS = -s -S                       # -s: GDB端口1234, -S: 启动时暂停

###########################################################################
# 构建目标
###########################################################################

.PHONY : build all clean link install qemurun qemudebug vboxrun gdb gdbrun dirs help

# 默认目标：显示帮助信息
.DEFAULT_GOAL := help

help:
	@echo "========================================"
	@echo "  Jeeos-0.01 Build System"
	@echo "========================================"
	@echo ""
	@echo "编译目标:"
	@echo "  make build      - 编译内核"
	@echo "  make install    - 编译并生成二进制内核"
	@echo "  make clean      - 清理编译文件"
	@echo ""
	@echo "运行目标:"
	@echo "  make qemurun    - 使用QEMU运行"
	@echo "  make qemudebug  - 使用QEMU调试模式 (GDB端口:1234)"
	@echo "  make vboxrun    - 使用VirtualBox运行"
	@echo ""
	@echo "调试帮助:"
	@echo "  make gdb        - 显示GDB连接命令"
	@echo "  make gdbrun     - 启动GDB调试脚本"
	@echo "========================================"

#---------------------------------------------------------------------------
# 主要构建目标
#---------------------------------------------------------------------------

# build: 清理后完整编译
build: clean dirs $(JEEOS_OBJS) link

# all: build的别名
all: build

# dirs: 创建必要的目录
dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(TOOLS_DIR)

# clean: 清理所有生成文件
clean:
	@echo "Clean everything ..."
	rm -rf $(BUILD_DIR)
	rm -f *.bin *.elf *.eki *.map *.img core.img hd.vdi
	rm -rf ./hdisk

###########################################################################
# 编译规则
###########################################################################

# 汇编: startup.asm -> startup.o
$(BUILD_DIR)/startup.o: $(BOOT_DIR)/startup.asm
	@echo "ASM -  $<"
	nasm $(NASMFLAGS) -o $@ $<

# C编译: main.c -> main.o
$(BUILD_DIR)/main.o: $(KERNEL_DIR)/main.c $(LIB_DIR)/console.h
	@echo "CC  -  $<"
	gcc $(CFLAGS) -o $@ $<

# C编译: console.c -> console.o
$(BUILD_DIR)/console.o: $(LIB_DIR)/console.c $(LIB_DIR)/console.h
	@echo "CC  -  $<"
	gcc $(CFLAGS) -o $@ $<

###########################################################################
# 链接和安装
###########################################################################

# link: 链接生成ELF可执行文件
link: $(BUILD_DIR)/JeeOS.elf

$(BUILD_DIR)/JeeOS.elf: $(JEEOS_OBJS)
	@echo "LD  -  JeeOS.elf"
	ld $(LDFLAGS) -o $@ $(JEEOS_OBJS)

# install: 转换ELF为纯二进制格式
install: $(BUILD_DIR)/JeeOS.bin

$(BUILD_DIR)/JeeOS.bin: $(BUILD_DIR)/JeeOS.elf
	@echo "BIN -  JeeOS.bin"
	objcopy -S -O binary $< $@

###########################################################################
# QEMU 运行
###########################################################################

# qemurun: 编译并使用QEMU运行
qemurun: build install
	@echo "========================================"
	@echo "  Jeeos-0.01 Running..."
	@echo "========================================"
	$(QEMU) -kernel $(BUILD_DIR)/JeeOS.bin $(QEMU_OPTS)

# qemudebug: QEMU调试模式，等待GDB连接
qemudebug: build install
	@echo "========================================"
	@echo "  Jeeos-0.01 Debug Mode"
	@echo "  GDB连接: target remote :1234"
	@echo "========================================"
	$(QEMU) -kernel $(BUILD_DIR)/JeeOS.bin $(QEMU_OPTS) $(QEMU_DEBUG_OPTS)

###########################################################################
# VirtualBox 运行
###########################################################################

# vboxrun: 使用VirtualBox运行
vboxrun: build install
	cd $(SCRIPTS_DIR) && ./vbox_setup.sh

###########################################################################
# GDB 调试
###########################################################################

# gdb: 显示GDB连接命令
gdb:
	@echo "========================================"
	@echo "  GDB 调试命令参考"
	@echo "========================================"
	@echo "  gdb"
	@echo "  (gdb) target remote :1234"
	@echo "  (gdb) file build/JeeOS.elf"
	@echo "  (gdb) break main"
	@echo "  (gdb) continue"
	@echo "========================================"

# gdbrun: 启动GDB调试脚本
gdbrun:
	$(TOOLS_DIR)/gdb_debug.sh
