#include <stdio.h>
#include <string.h>
#include "rb_tree.h"
#include "vma.h"

void vma_link_test()
{
	struct mm_struct *mm;
	struct vm_area_struct *vma, *prev;
	struct rb_node **rb_link, *rb_parent;
	unsigned long addr, len;

	mm = mm_alloc();

	vma = vm_area_alloc(mm);
	addr = 0x1000;
	len  = 0x1000;
	vma->vm_start = addr;
	vma->vm_end   = addr + len;

	find_vma_links(mm, addr, addr + len, &prev, &rb_link, &rb_parent);
	printf("prev is %p\n", prev);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	vma = vm_area_alloc(mm);
	addr += 0x1000;
	len  = 0x1000;
	vma->vm_start = addr;
	vma->vm_end   = addr + len;

	find_vma_links(mm, addr, addr + len, &prev, &rb_link, &rb_parent);
	printf("prev is %p\n", prev);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	printf("\nDump vma tree: \n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);

	printf("\nIterate vma list: \n");
	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		printf("0x%08lx - 0x%08lx\n", vma->vm_start, vma->vm_end);
	}
}

int main()
{
	vma_link_test();
	return 0;
}
