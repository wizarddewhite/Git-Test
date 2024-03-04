本目录记录了使用bochs调试最小内核的笔记。

# 安装软件

下面的命令行是在ubuntu环境下执行的，并且已经安装了基本的开发工具。

```
sudo apt install bochs
sudo apt install bochs-x
sudo apt install nasm
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

显示一段，比如之观察512(0x200)字节位置是否是0x55AA。

```
hexdump -C -s 0x180 -n 0x100 boot.bin
```

# 用bochs调试

## 查看帮助

```
help
help pb
help info
```

## 断点设置

这里的地址是物理地址
```
pb 0x7c00
```

查看已设置的断点
```
info break
```

## 反汇编

反汇编某一段地址间的代码
```
u 0x7c00 0x7c1e
```

反汇编接下来的5条指令
```
u /5
```

## 查看寄存器
```
sreg
r
```

## 查看内存

从地址0x000916ed显示后续8个word。

```
xp /8xw 0x000916ed
```
