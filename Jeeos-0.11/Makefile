###########################################################################
# Jeeos-0.11 主构建文件
#
# 功能: 编译中级操作系统框架、生成镜像、运行虚拟机
#
# 项目结构:
#   arch/x86/     - 架构相关代码 (boot, kernel)
#   kernel/       - 内核核心代码
#   system/       - 系统调用层
#   libs/         - 公共库
#   script/       - 构建脚本
#   tools/        - 工具程序
#   res/          - 资源文件 (字体、图片)
#
# 构建流程:
#   1. buildtool  - 编译构建工具(buildtool)
#   2. boot       - 编译引导程序(bootsect, setup, bootparam)
#   3. prebuild   - 预处理构建
#   4. archkernel - 编译架构层(HAL)
#   5. kernel     - 编译内核
#   6. system     - 编译系统调用层
#   7. link       - 链接生成jeeos.bin
#   8. buildtool  - 打包生成jeeos.eki
#
# 主要目标:
#   build     - 编译内核
#   install   - 安装到release目录
#   qemurun   - QEMU运行
#   qemudebug - QEMU调试模式 (GDB端口1234)
#   vboxrun   - VirtualBox运行
###########################################################################

MAKEFLAGS = -sR

#---------------------------------------------------------------------------
# 目录定义
#---------------------------------------------------------------------------
ARCH_X86_ROOT = ./arch/x86

# 编译输出目录
BUILD_PATH = ./build
# 发布目录
RELEASE_PATH = ./build/release
# VirtualBox相关文件目录
VBOX_PATH = ./build/vbox
# 工具目录
TOOLS_PATH = ./tools
# buildtool源码目录
BUILDTOOL_PATH = ./tools/buildtool
# 构建脚本目录
SCRIPT_PATH = ./script
# 引导程序目录
BOOT_PATH = ${ARCH_X86_ROOT}/boot
# 资源文件目录
RESOURCES_PATH = ./res
# 字体文件目录
FONT_PATH = ./res/fonts
# 图片文件目录
IMAGE_PATH = ./res/images

###########################################################################
# 构建目标
###########################################################################

.PHONY : build print clean

#---------------------------------------------------------------------------
# 主要构建目标
#---------------------------------------------------------------------------

# build: 清理后完整编译
build: clean all

# all: 完整编译流程
#   1. 编译buildtool工具
#   2. 创建编译输出目录并复制构建脚本
#   3. 编译引导程序
#   4-9. 依次编译prebuild, archkernel, kernel, system, drivers
#   10. 链接生成jeeos.bin
all:
	cd $(BUILDTOOL_PATH); make
	@echo "Created directory $(BUILD_PATH) "
	mkdir -p $(BUILD_PATH)
	cp -r $(SCRIPT_PATH)/*.mkf $(SCRIPT_PATH)/mkftool $(BUILD_PATH)/
	cd ./arch/x86/boot/ && make
	make -C ./build/ -f prebuild.mkf
	make -C ./build/ -f archkernel.mk
	make -C ./build/ -f kernel.mk
	make -C ./build/ -f system.mk
	make -C ./build/ -f drivers.mk
	make -C ./build/ -f libs.mk
	make -C ./build/ -f link.mk

# clean: 清理所有生成文件
clean:
	@echo "Clean everything ..."
	-cd $(BOOT_PATH) && make clean
	-cd $(BUILDTOOL_PATH) && make clean
	-@if [ -d $(BUILD_PATH) ]; then cd $(BUILD_PATH) && rm -f *.o *.bin *.i *.krnl *.s *.map *.lib *.btoj *.vdi *.elf *vmdk *.lds *.mk *.mki krnlobjs.mh *.mkf mkftool; fi
	-@if [ -d $(RELEASE_PATH) ]; then cd $(RELEASE_PATH) && rm -f *.o *.bin *.i *.krnl *.s *.eki *.map *.lib *.btoj *.elf *.vdi *vmdk *.lds *.mk *.mki *.bmp *.fnt *.img krnlobjs.mh; fi
	-@if [ -d $(BUILD_PATH) ]; then cd $(BUILD_PATH) && rm -rf release vbox; fi

#---------------------------------------------------------------------------
# 安装目标
#---------------------------------------------------------------------------

# install: 编译并安装到release目录
install: build checkdir cpres cpboot cpkernel cptools

# checkdir: 创建必要目录
checkdir:
	@echo "Created directory $(RELEASE_PATH) "
	mkdir -p $(RELEASE_PATH)
	@echo "Created directory $(VBOX_PATH) "
	mkdir -p $(VBOX_PATH)

# cpres: 复制资源文件
cpres:
	@echo "MakeFile cp font files to release"
	-cp $(FONT_PATH)/* -r $(RELEASE_PATH)
	@echo "MakeFile cp image files to release"
	-cp $(IMAGE_PATH)/* -r $(RELEASE_PATH)

# cpboot: 复制引导文件
cpboot:
	@echo "MakeFile cp boot files to release"
	cp $(CPFLAGES) $(BOOT_PATH)/build/bootsect.bin $(BOOT_PATH)/build/setup.bin $(BOOT_PATH)/build/bootparam.bin $(RELEASE_PATH)

# cpkernel: 复制内核文件
cpkernel:
	@echo "MakeFile cp kernel files to release"
	cd $(BUILD_PATH) && cp $(CPFLAGES) jeeos.bin ../build/release

# cptools: 复制工具文件
cptools:
	@echo "MakeFile cp build tools files to release"
	cd $(BUILDTOOL_PATH) && cp ./build/buildtool ../../build/release

###########################################################################
# VirtualBox 运行
###########################################################################

# vboxrun: 使用VirtualBox运行
vboxrun: install mkhdisk mkimg mkvbox

# mkvbox: 启动VirtualBox
mkvbox:
	make -C $(BUILD_PATH) -f vbox.mkf

###########################################################################
# QEMU 运行
###########################################################################

# qemurun: 使用QEMU运行
qemurun: install mkhdisk mkimg mkqemu

# mkqemu: 启动QEMU
mkqemu:
	make -C $(BUILD_PATH) -f qemu.mkf all

# qemudebug: QEMU调试模式，启动后等待GDB连接到1234端口
qemudebug: install mkhdisk mkimg
	make -C $(BUILD_PATH) -f qemu.mkf debug

###########################################################################
# 镜像生成
###########################################################################

# mkhdisk: 创建虚拟硬盘
mkhdisk:
	cd $(TOOLS_PATH) && ./mk_hdisk.sh

# mkimg: 使用buildtool打包内核镜像
mkimg:
	@echo 'Installing Jeeos kernel, waitting......'
	cd ./build/release/ && ./buildtool -m k -lhf bootsect.bin -o jeeos.eki -f setup.bin bootparam.bin jeeos.bin font.fnt logo.bmp background.bmp

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
	@echo "  (gdb) file build/jeeos.elf"
	@echo "  (gdb) break start_kernel"
	@echo "  (gdb) continue"
	@echo "========================================"

# gdbrun: 启动GDB调试脚本
gdbrun:
	$(TOOLS_PATH)/gdb_debug.sh

###########################################################################
# 帮助信息
###########################################################################

help:
	@echo "========================================"
	@echo "  Jeeos-0.11 Build System"
	@echo "========================================"
	@echo ""
	@echo "编译目标:"
	@echo "  make build      - 编译内核"
	@echo "  make install    - 编译并安装到release目录"
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
