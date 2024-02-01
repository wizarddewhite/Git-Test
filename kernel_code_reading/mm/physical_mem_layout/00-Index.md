This directory contains the records when learning linux MM system. And the
file here is the INDEX of the files in this directory.

1. Memory management hierarchy
1.1 <a href="./e820.txt">e820.txt</a>

e820 is the lowest level hardware who know the exact information of the memory
range. Software gets memory range information from e820.

1.2 memblock.txt

memblock is more a software concept, which hide memory range information
exported from hardware like e820 and form the memblock information and gives
it to higher level.

1.3 nobootmem.txt

Before MM system sets up, system manage memory by bootmem. In some case, this
is the same thing as memblock.

1.4 page_alloc.txt

memblock contains generally information from hardware, which means it is not
normalized. Linux kernel use structure page to normalized them.

In this file, it focus on the page_alloc and page_free.

1.5 mmzone.txt

1.6 fork.txt vma.txt

Each process has its own address space view, which is described by struct
mm_struct, especially with the field struct vm_area_struct.

fork.txt includes some functions on mm_struct init.
vma.txt focus on helper functions on vm_area_struct.

2. procfs and sysfs

2.1 meminfo.txt

/proc/meminfo

2.2 proc_maps.txt

/proc/self/maps