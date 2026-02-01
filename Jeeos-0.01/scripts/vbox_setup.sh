#!/bin/bash
# ============================================
# JeeOS 虚拟机启动脚本
# ============================================
# 使用说明:
# 1. 运行脚本之前，在VirtualBox中创建JeeOS虚拟机，配置如下:
#     JeeOS配置：
#         Memory:       64M
#         Type:         Other
#         Virsion:      Other/Unknown(64-bit)
#         Hard disk:    Do not add a virtual hard disk
# 2. 在项目根目录运行: make vboxrun
# ============================================

set -e  # 遇到错误立即退出

# 获取脚本所在目录和项目根目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"

# 切换到项目根目录执行
cd "$PROJECT_ROOT"

echo "============================================"
echo "JeeOS VM Setup Script"
echo "============================================"
echo "Project root: ${PROJECT_ROOT}"
echo "Build dir: ${BUILD_DIR}"

# 检查必要文件
if [ ! -f "${BUILD_DIR}/JeeOS.bin" ]; then
    echo "Error: ${BUILD_DIR}/JeeOS.bin not found!"
    echo "Please run 'make all install' first."
    exit 1
fi

LOOP_PATH=$(sudo losetup -f)
echo "Get the loop device path : ${LOOP_PATH}"

VM_NAME="JeeOS"
H_DISK="hdisk"
GRUB_CFG_PATH=${H_DISK}/boot/grub/grub.cfg

# 先从 VirtualBox 卸载并注销旧的虚拟硬盘
VBoxManage storageattach ${VM_NAME} --storagectl "SATA" --port 1 --device 0 --type hdd --medium none 2>/dev/null || true
VBoxManage closemedium disk "${PROJECT_ROOT}/hd.vdi" --delete 2>/dev/null || true
echo "Detached and removed old virtual disk from VirtualBox."

# 清理旧文件
rm -f core.img hd.vdi
rm -rf ${H_DISK}

# bs:表示块大小，这里是512字节
# if：表示输入文件，/dev/zero就是Linux下专门返回0数据的设备文件
# of：表示输出文件，即我们的硬盘文件
# count：表示输出多少块
dd bs=512 if=/dev/zero of=core.img count=20480
echo "dd command creates core.img of 10M size."

# 把虚拟硬盘文件变成 Linux 下的回环设备
sudo losetup ${LOOP_PATH} core.img
echo "losetup command mounts core.img as a block device into : ${LOOP_PATH}."

# mkfs.ext4 命令格式化这个回环块设备，在里面建立 EXT4 文件系统
sudo mkfs.ext4 -q ${LOOP_PATH}
echo "Format ${LOOP_PATH} as EXT4 file system."

# 创建hdisk目录
mkdir ${H_DISK}
echo "mkdir ${H_DISK}"

# 挂载硬盘文件到hdisk
sudo mount -o loop ./core.img ${H_DISK}
echo "mount core.img to hdisk."

# hdisk中建立boot目录
sudo mkdir ${H_DISK}/boot
echo "mkdir ${H_DISK}/boot"

# 通过 GRUB 的安装程序，把 GRUB 安装到指定的设备上
sudo grub-install --boot-directory=${H_DISK}/boot --force --allow-floppy --target=i386-pc ${LOOP_PATH}
echo "grub installing!"
sleep 2

sudo touch ${GRUB_CFG_PATH}
sudo chmod 766 ${GRUB_CFG_PATH}
echo "create ${GRUB_CFG_PATH}"

# 创建grub.cfg
sudo cat << "EOF" > ${GRUB_CFG_PATH}
menuentry 'JeeOS' {
    insmod part_msdos
    insmod ext2
    set root='hd0'
    multiboot /boot/JeeOS.bin
    boot
}
set timeout_style=menu
set timeout=10
EOF
sleep 2

sudo chmod 644 ${GRUB_CFG_PATH}
echo "Write bootloader code to ${GRUB_CFG_PATH}"
sleep 2

# 复制内核文件
echo "cp ${BUILD_DIR}/JeeOS.bin to ./${H_DISK}/boot/"
sudo cp "${BUILD_DIR}/JeeOS.bin" ./${H_DISK}/boot/
sleep 2

# 释放循环设备
sudo losetup -d ${LOOP_PATH}
echo "Delete ${LOOP_PATH}"

# 卸载hdisk
sudo umount ${H_DISK}
echo "Umount ${H_DISK}!"

# 转换为 VDI 格式
VBoxManage convertfromraw ./core.img --format VDI ./hd.vdi
echo "Convert core.img to format hd.vdi!"
sleep 1

# 为新的 VDI 文件设置新的 UUID（避免 UUID 冲突）
VBoxManage internalcommands sethduuid ./hd.vdi
echo "Assigned new UUID to hd.vdi."
sleep 1

# 配置 SATA 控制器（如果不存在则创建）
VBoxManage storagectl ${VM_NAME} --name "SATA" --add sata --controller IntelAhci --portcount 2 2>/dev/null || true
echo "Set up the SATA hard drive controller."
sleep 1

# 将虚拟硬盘挂到虚拟机的硬盘控制器
VBoxManage storageattach ${VM_NAME} --storagectl "SATA" --port 1 --device 0 --type hdd --medium "${PROJECT_ROOT}/hd.vdi"
echo "Mount the virtual hard disk to the vm hard disk controller."

# 启动虚拟机
VBoxManage startvm ${VM_NAME}
echo "============================================"
echo "JeeOS VM is starting!"
echo "============================================"
