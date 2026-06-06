# Readme
###################################################################################################
# 1. 运行脚本之前，在VirtualBox中创建Jeeos虚拟机，配置如下:
#     Jeeos配置：
#         Memory:       1024M
#         Type:         Other
#         Virsion:      Other/Unknown(64-bit)
#         Hard disk:    Do not add a virtual hard disk
# 2. 创建自己的实验目录，并将mk_bootloader.sh拷贝到自己的目录
# 3. chmod 777 mk_bootloader.sh
# 4. ./mk_bootloader.sh
###################################################################################################

# echo $(sudo rm -rf ./*)
# echo "Clear all files and the environment is ready."
LOOP_PATH=$(sudo losetup -f)
echo "Get the loop device path : ${LOOP_PATH}"

VM_NAME="Jeeos"
H_DISK="hdisk"
GRUB_CFG_PATH=${H_DISK}/boot/grub/grub.cfg

# bs:表示块大小，这里是512字节
# if：表示输入文件，/dev/zero就是Linux下专门返回0数据的设备文件，读取它就返回0
# of：表示输出文件，即我们的硬盘文件
# count：表示输出多少块
dd bs=512 if=/dev/zero of=hd.img count=204800
echo "dd command creates hd.img of 100M size."

# 把虚拟硬盘文件变成 Linux 下的回环设备
# losetup -a 查看所有/dev/loop
# losetup -f 返回一个可用的/dev/loop
sudo losetup ${LOOP_PATH} hd.img
echo "losetup command mounts hd.img as a block device into : ${LOOP_PATH}."
# mkfs.ext4 命令格式化这个 /dev/loop13 回环块设备，在里面建立 EXT4 文件系统
sudo mkfs.ext4 -q ${LOOP_PATH}
echo "Format ${LOOP_PATH} as EXT4 file system."

# 创建hdisk目录
mkdir ${H_DISK}
echo "mkdir ${H_DISK}"

# 挂载硬盘文件到hdisk
sudo mount -o loop ./hd.img ${H_DISK}
echo "mount hd.img to hdisk."
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

# 创建grub.cfg，将如下代码内容写入grub.cfg
sudo cat << "EOF" > ${GRUB_CFG_PATH}
menuentry 'HelloOS' {
insmod part_msdos
insmod ext2
set root='hd0' #我们的硬盘只有一个分区所以是'hd0,msdos1'
multiboot2 /boot/HelloOS.eki #加载boot目录下的HelloOS.eki文件
boot #引导启动
}
set timeout_style=menu
if [ "${timeout}" = 0 ]; then
  set timeout=10 #等待10秒钟自动启动
fi
EOF
sleep 2

sudo chmod 644 ${GRUB_CFG_PATH}
echo "Write bootloader code to ${GRUB_CFG_PATH}"
sleep 10

# convertfromraw 指向原始格式文件
# --format VDI  表示转换成虚拟需要的VDI格式
VBoxManage convertfromraw ./hd.img --format VDI ./hd.vdi
echo "Convert hd.img to formart hd.vdi!"
sleep 2

# SATA的硬盘其控制器是intelAHCI
VBoxManage storagectl ${VM_NAME} --name "SATA" --add sata --controller IntelAhci --portcount 1
echo "Set up the SATA hard drive and its controller is intelAHCI."
sleep 2
# 删除虚拟硬盘UUID并重新分配
VBoxManage closemedium disk ./hd.vdi
echo "Delete virtual hard disk UUID and reassign."
sleep 2
# 将虚拟硬盘挂到虚拟机的硬盘控制器
VBoxManage storageattach ${VM_NAME} --storagectl "SATA" --port 1 --device 0 --type hdd --medium ./hd.vdi
echo "Mount the virtual hard disk to the vm hard disk controller."

# 这个命令会释放已经占用的循环设备，并取消与文件的关联，即解除挂载
sudo losetup -d ${LOOP_PATH}
echo "Delete ${LOOP_PATH}"
# 卸载hdisk
sudo umount ${H_DISK}
echo "Umount ${H_DISK}!"

# 启动虚拟机
VBoxManage startvm ${VM_NAME}
echo "VM is starting !!!"





