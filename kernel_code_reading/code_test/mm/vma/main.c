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

/*
 *
 *               a
 *               [0x9000, 0x10000]
 *           /            \
 *       n                 b
 *       [0x8000, 0x9000]  [0x10000, 0x11000]
 *    /
 *   vma
 *   [0x6000, 0x7000]
 *
 */
void vma_gap_test()
{
	struct mm_struct *mm;
	struct vm_area_struct *vma, *prev;
	struct rb_node **rb_link, *rb_parent;
	unsigned long addr, len;

	mm = mm_alloc();

	/* Node a [0x9000, 0x10000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x9000;
	vma->vm_end   = 0x10000;
	find_vma_links(mm, 0x9000, 0x10000, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node n [0x8000, 0x9000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x8000;
	vma->vm_end   = 0x9000;
	find_vma_links(mm, 0x8000, 0x9000, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node b [0x10000, 0x11000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x10000;
	vma->vm_end   = 0x11000;
	find_vma_links(mm, 0x10000, 0x11000, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);
	printf("\nDump vma tree: \n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);

	vma = vm_area_alloc(mm);
	vma->vm_start = 0x6000;
	vma->vm_end   = 0x7000;
	find_vma_links(mm, 0x6000, 0x7000, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	printf("\nDump vma tree: \n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);
	printf("\nsubtree gap computed: %lu\n", subtree_compute_count);
}

int main()
{
	vma_gap_test();
	return 0;
}
