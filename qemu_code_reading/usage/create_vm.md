初次接触虚拟化，我们就当作是台物理机一样给他装机吧。

# 安装一台虚拟机

## 事前准备

就和我们要安装物理机一样，我们在安装虚拟机之前也要做点事先的准备。要准备这么几样东西。

* 必要的系统检查
* 创建虚拟机的虚拟磁盘
* 下载guest系统的iso镜像

### 必要的系统检查

检查分为两种，硬件和软件，软件上又可以分为内核模块和应用程序。

首先芯片上要支持kvm，比如x86的架构需要有vmx。这个在linux系统上通过命令“cat /proc/cpuinfo | grep vmx”来判断。如果有这个标志，说明芯片是支持的。

然后看内核是否支持。通过命令“lsmod | grep kvm”。如果看到有两个模块kvm, kvm_intel，那么表示当前的内核已经加载了这两个模块。如果没有加载，请加载或者需要重新编译内核。

最后看有没有qemu安装。这个每个发行版的可执行文件的名字或许有差别，但几本都以qemu开头。有的叫qemu-kvm, qemu-system-x86_64等。可以使用包管理软件搜索qemu来安装。

### 创建虚拟机的虚拟磁盘

这个就和我们装物理机，要求物理机上有硬盘一样。我们安装的虚拟机也是要有虚拟磁盘的。

虚拟磁盘有多种形式和格式，这里就不展开了。简单来讲，虚拟磁盘在host上就是一个文件。我们可以用以下命令创建一个虚拟磁盘。

```
qemu-img create -f raw fedora.img 30G
```

这样就在当前目录下创建了一个大小为30G的虚拟磁盘文件fedora.img。这个虚拟磁盘的格式是raw的。关于格式，不在这里介绍。有兴趣的童鞋可以自行查阅资料。

### 下载guest系统的iso镜像

你喜欢装什么系统就下什么系统吧。和你主机一样的也行，能装就行。

## 开始安装

命令很简单
```
qemu-kvm -m 4096 -smp 4 -hda fedora.img -cdrom Fedora.iso -boot dc
```

就这么简单，这一条命令就可以启动虚拟机并进入安装的界面了。

简单介绍一下各个参数

-m 4096:    虚拟机内存是4096MB
-smp 4:      虚拟机有4个vcpu
-hda:          硬盘的文件
-cdrom:     光盘文件
-boot:        启动顺序，从光盘启动

启动后就能看到和真实物理机安装一样的界面。按照提示一步步往下走就好了。现在的发行版已经做得很人性化了。

# 启动虚拟机

启动也很简单，一条命令。

命令很简单
```
qemu-kvm -m 4096 -smp 4 -hda fedora.img
```

就是把cdrom和启动顺序的参数去掉就可以了～

好了，欢迎来到虚拟的世界。

# 小进阶 -- 通过字符界面安装虚拟机

有的时候我们没有图形界面，或者无法很好地使用虚拟机。比如我们在服务器上安装虚拟机，很大情况下是不安装图形桌面的，或者网络带宽受到限制，使用图形界面会让你崩溃。

好了，小编倾情奉献准独家秘籍，通过字符界面安装虚拟机。

备注： 该方法不一定适合所有发行版。下面的例子安装的是rhel6.7。

## 挂载虚拟机iso

挂载iso是为了找到其中的kernel和initrd。
```
sudo mount -o loop rhel-server-6.7-x86_64-dvd.iso tmp/
```

## 找到 启动kernel 和 initrd

一般情况下启动的kernel和initrd在光盘文件的isolinux目录内。

比如在rhel6.7种，文件本别是

> isolinux/vmlinuz
> isolinux/initrd.img

路径和名字若有变化，请自行探索。

## 开始安装

好了，这就要开始安装了哈。我使用的命令是：

```
sudo qemu-system-x86_64 --enable-kvm -m 4096 -smp 4 \
ubuntu.img \
-cdrom rhel-server-6.7-x86_64-dvd.iso \
--nographic \
-append console=ttyS0 \
-kernel tmp/isolinux/vmlinuz \
-initrd tmp/isolinux/initrd.img
```

前面和正常安装的是一样的，就是多了后面四个选项。

```
--nographic  不使用图形界面
--append     启动时传给内核的参数
--kernel     使用哪个内核
--initrd     使用哪个initrd 
```

# 再进一步--一键安装

好了，懒人的天性要发挥一下。虽然现在的安装界面已经很友好，步骤也很简单了，但是如果能够一键安装，世界会是有多么美好。

嘿，这事儿还真是有。人早就做好这个了。
这个东西叫做kickstart。

使用kickstart的方式有几种，我这里提供一种把脚本放到iso文件的方式。其余的方式大家可以网上自行搜索。

## 编写kickstart脚本

kickstart就是一个事先写好的脚本，里面写上了安装过程中的信息。这里给出一个可以安装的例子。

PS: kickstart主要用在RHEL和CentOS。

```
# This is an installation
install
# From cdrom
cdrom
lang en_US.UTF-8
keyboard us
# root password, plaintest
#rootpw --plaintext abc123
# root password encrypted
rootpw --iscrypted $6$27qsH/RZygFeKCF9$iDAZJsCnN7N
firewall --service=ssh
autoconfig --enableshadow --passalog=sha512 --enablefingerprint
selinux --enforcing
# power off after installation
poweroff
timezone --utc America/New_York
bootloader --location=mbr --driverorder=sda --append="console=ttyS0 crashkernel=auto"
# clear the disk
clearpart --all --drivers=sda --initlabel
autopart
zerombr
repo --name="Red Hat Enterprise Linux" --baseurl=file:///mnt/source --cost=100

%packages --nobase
@core

%end
```

整个脚本也还好，有些不是特别明白，但是基本的东西能看懂。有几个点特别注释一下：

*  poweroff  这个有几个选项，reboot, halt。试了一下，只有在poweroff的时候安装完qemu是退出的，其他的都会卡在那。
* autopart 一定要加上，否则还是会提示你要怎么分区

大家看了这个kickstart脚本估计还是会不知道要怎么写。告诉大家两个可以借鉴别人例子的地方。

* RHEL手动安装完后会有一个脚本在/root目录下，上面这个例子就是从这个文件拷贝出来的
* RHEL带有一个配置的软件， system-config-kickstart。有些选项不知道怎么写，可以借鉴这个软件的。


## 制作带有kiskstart脚本的iso

这个步骤好简单的～

> mkisofs -o ks.iso ks.cfg

这样你就有了一个带有kickstart脚本的iso。

脚本的文件名一定是ks.cfg
脚本的文件名一定是ks.cfg
脚本的文件名一定是ks.cfg

反正我改成这个名字才装成功了。

## 传说的一键安装

好了，开始安装吧。其实就是一条命令～

```
sudo qemu-system-x86_64 --enable-kvm -m 4096 -smp 4 \
ubuntu.img \
-drive file=rhel-server-6.7-x86_64-dvd.iso,media=cdrom,index=1 \
-drive file=ks.iso,media=cdrom,index=2 \
--nographic \
-append "console=ttyS0 ks=cdrom:/ks.cfg"\
-kernel tmp/isolinux/vmlinuz \
-initrd tmp/isolinux/initrd.img
```

Hope you enjoy it~
