# 内存布局

          +----------------------------+
          |                            |
          .                            .
          |                            |
          +----------------------------+
          |  Page Tables               |
          |                            |
0x101000  +----------------------------+
          |  Page Directory Table      |
0x100000  +----------------------------+
          |                            |
          .                            .
          |                            |
          +----------------------------+
          |  Loader.bin                | 2. 加载kernel.bin到0x80000
          |                            |    进入保护模式
          |                            |    在0x100000建立页表
 0x90100  +----------------------------+
          |                            |
          .                            .
          |                            |
          +----------------------------+
          |  Kernel.bin                |
          |                            |
 0x80000  +----------------------------+
          |                            |
          .                            .
          |                            |
 0x07e00  +----------------------------+
          |  Boot.bin                  | 1. 引导扇区被加载到这里
          |                            |    找到Loader.bin，加载到0x90100h
          |                            |    并跳转到0x90100h
 0x07c00  +----------------------------+
          |                            |
          .                            .
          |                            |
 0x00000  +----------------------------+

# readelf查看elf信息

查看ELF Header

```
readelf -h file
```

查看Program Header

# ld -T指定入口地址

```
ld -Ttext addr
```

通过readelf -h -l xxx可以看到变化

没有加选项时
```
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  ...
  Type:                              EXEC (Executable file)
  Machine:                           Intel 80386
  Version:                           0x1
  Entry point address:               0x8049000
  ...
  Section header string table index: 7

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000000 0x08048000 0x08048000 0x000d4 0x000d4 R   0x1000
  LOAD           0x001000 0x08049000 0x08049000 0x00088 0x00088 R E 0x1000
  LOAD           0x002000 0x0804a000 0x0804a000 0x0006c 0x0006c R   0x1000
  LOAD           0x003000 0x0804c000 0x0804c000 0x00014 0x00014 RW  0x1000
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x10
```

增加-Ttext 0x30400选项时
```
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  ...
  Type:                              EXEC (Executable file)
  Machine:                           Intel 80386
  Version:                           0x1
  Entry point address:               0x30400
  ...
  Section header string table index: 7

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000000 0x00030000 0x0002f000 0x000d4 0x000d4 R   0x1000
  LOAD           0x000400 0x00030400 0x00030400 0x00088 0x00088 R E 0x1000
  LOAD           0x001000 0x00031000 0x00031000 0x0006c 0x0006c R   0x1000
  LOAD           0x002000 0x00033000 0x00033000 0x00014 0x00014 RW  0x1000
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x10
```
