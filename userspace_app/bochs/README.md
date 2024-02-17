本目录记录了使用bochs调试最小内核的笔记。

# 安装软件

下面的命令行是在ubuntu环境下执行的，并且已经安装了基本的开发工具。

```
sudo apt install bochs
sudo apt install bochs-x
sudo apt install nasm
```

# 运行例子

先编译

```
make
```

在执行

```
make run

或者

./vm_start.sh
```

# 查看二进制文件

用vim
```
vim -b boot.bin
:%!xxd
```

用hexdump

```
hexdump -C boot.bin
```
