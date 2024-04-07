#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
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

	/* Node vma [0x6000, 0x7000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x6000;
	vma->vm_end   = 0x7000;
	find_vma_links(mm, 0x6000, 0x7000, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	printf("\nDump vma tree: \n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);
	printf("\nsubtree gap computed: %lu\n", subtree_compute_count);

	/* erase vma [0x6000, 0x7000] */
	//__vma_unlink_common
	vma_rb_erase_ignore(vma, &mm->mm_rb, NULL);
	__vma_unlink_list(mm, vma);
	printf("\nDump vma tree: Not Consistent\n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);

	/* After update vma->vm_next, rb_subtree_gap is correct */
	vma_gap_update(vma->vm_next);
	printf("\nDump vma tree after update next gap: Consistent\n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);
	printf("\nsubtree gap computed: %lu\n", subtree_compute_count);

}

void vma_gap_remove_test()
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

	/* Node vma [0x6000, 0x7000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x6000;
	vma->vm_end   = 0x7000;
	find_vma_links(mm, 0x6000, 0x7000, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	printf("\nDump vma tree: \n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);
	printf("\nsubtree gap computed: %lu\n", subtree_compute_count);

	/*
	 * erase vma [0x6000, 0x7000]
	 *
	 * Compared with vma_gap_test(), we reorder __vma_unlink_list and
	 * vma_rb_erase_ignore(). After doing so, one time less of
	 * subtree_compute_count.
	 */
	//__vma_unlink_common
	subtree_compute_count = 0;
	__vma_unlink_list(mm, vma);
	vma_rb_erase_ignore(vma, &mm->mm_rb, vma->vm_next);
	vma_gap_update(vma->vm_next);
	printf("\nDump vma tree: Consistent\n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);
	printf("\nsubtree gap computed: %lu\n", subtree_compute_count);
	validate_mm_rb(&mm->mm_rb, NULL);
}

/* always remove root */
void vma_gap_remove_test2()
{
	struct mm_struct *mm;
	struct vm_area_struct *vma, *prev;
	struct rb_node **rb_link, *rb_parent;
	unsigned long addr, len;

	mm = mm_alloc();

	/* Node a [0x10000, 0x11000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x10000;
	vma->vm_end   = 0x11000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node n [0x8000, 0x9000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x8000;
	vma->vm_end   = 0x9000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node b [0x13000, 0x15000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x13000;
	vma->vm_end   = 0x15000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node vma [0x6000, 0x7000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x6000;
	vma->vm_end   = 0x7000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node [0x9000, 0x10000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x9000;
	vma->vm_end   = 0x10000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node [0x11000, 0x13000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x11000;
	vma->vm_end   = 0x13000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node [0x15000, 0x17000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x15000;
	vma->vm_end   = 0x17000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	printf("\nDump vma tree: \n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);

	while (mm->mm_rb.rb_node) {
		vma = rb_entry(mm->mm_rb.rb_node, struct vm_area_struct, vm_rb);
		__vma_unlink_list(mm, vma);
		vma_rb_erase_ignore(vma, &mm->mm_rb, vma->vm_next);
		printf("\n\nDump vma tree: after [0x%08lx - 0x%08lx]\n",
				vma->vm_start, vma->vm_end);
		dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);
	}

}

/* always remove right child of root if has */
void vma_gap_remove_test3()
{
	struct mm_struct *mm;
	struct vm_area_struct *vma, *prev;
	struct rb_node **rb_link, *rb_parent;
	unsigned long addr, len;
	int level;

	mm = mm_alloc();

	/* Node a [0x10000, 0x11000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x10000;
	vma->vm_end   = 0x11000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node n [0x8000, 0x9000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x8000;
	vma->vm_end   = 0x9000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node b [0x13000, 0x15000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x13000;
	vma->vm_end   = 0x15000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node vma [0x6000, 0x7000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x6000;
	vma->vm_end   = 0x7000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node [0x9000, 0x10000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x9000;
	vma->vm_end   = 0x10000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node [0x11000, 0x13000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x11000;
	vma->vm_end   = 0x13000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node [0x15000, 0x17000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x15000;
	vma->vm_end   = 0x17000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	printf("\nDump vma tree: \n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);

	srand(time(NULL));

	while (mm->mm_rb.rb_node) {
		int l, dir;
		struct rb_node *node;

		level = rand() % 3;
		node = mm->mm_rb.rb_node;
		for (l = 0; l < level; l++) {
			dir = rand() % 2;
			if (node->rb_right)
				node = node->rb_right;
			else
				break;
		}

		vma = rb_entry(node, struct vm_area_struct, vm_rb);
		printf("\n\nTry to remove [0x%08lx - 0x%08lx] next is %p\n",
				vma->vm_start, vma->vm_end, vma->vm_next);
		__vma_unlink_list(mm, vma);
		vma_rb_erase_ignore(vma, &mm->mm_rb, vma->vm_next);
		validate_mm_rb(&mm->mm_rb, NULL);
		printf("Dump vma tree: after remove [0x%08lx - 0x%08lx]\n",
				vma->vm_start, vma->vm_end);
		dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);
	}
}

void vma_gap_remove_failure()
{
	struct mm_struct *mm;
	struct vm_area_struct *vma, *prev;
	struct rb_node **rb_link, *rb_parent;
	unsigned long addr, len;

	mm = mm_alloc();

	/* Node a [0x10000, 0x11000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x10000;
	vma->vm_end   = 0x11000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node n [0x8000, 0x9000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x8000;
	vma->vm_end   = 0x9000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node b [0x13000, 0x15000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x13000;
	vma->vm_end   = 0x15000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node vma [0x6000, 0x7000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x6000;
	vma->vm_end   = 0x7000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node [0x9000, 0x10000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x9000;
	vma->vm_end   = 0x10000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node [0x11000, 0x13000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x11000;
	vma->vm_end   = 0x13000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	/* Node [0x15000, 0x17000] */
	vma = vm_area_alloc(mm);
	vma->vm_start = 0x15000;
	vma->vm_end   = 0x17000;
	find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent);
	vma_link(mm, vma, prev, rb_link, rb_parent);

	printf("\nDump vma tree: \n");
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);

	vma = rb_entry(mm->mm_rb.rb_node, struct vm_area_struct, vm_rb);
	__vma_unlink_list(mm, vma);
	vma_rb_erase_ignore(vma, &mm->mm_rb, vma->vm_next);
	validate_mm_rb(&mm->mm_rb, NULL);
	printf("\n\nDump vma tree: after [0x%08lx - 0x%08lx]\n",
			vma->vm_start, vma->vm_end);
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);

	vma = rb_entry(mm->mm_rb.rb_node->rb_right, struct vm_area_struct, vm_rb);
	__vma_unlink_list(mm, vma);
	printf("vma : [0x%08lx - 0x%08lx]\n", vma->vm_start, vma->vm_end);
	printf("next: [0x%08lx - 0x%08lx]\n", vma->vm_next->vm_start, vma->vm_next->vm_end);
	vma_rb_erase_ignore(vma, &mm->mm_rb, vma->vm_next);

	// for this case, vma->vm_left is NULL, we still need to update
	// vm_next
	vma_gap_update(vma->vm_next);

	validate_mm_rb(&mm->mm_rb, NULL);
	printf("\n\nDump vma tree: after [0x%08lx - 0x%08lx]\n",
			vma->vm_start, vma->vm_end);
	dump_rb_tree(mm->mm_rb.rb_node, 0, root_node, vma_print);
}

int main()
{
	vma_gap_remove_test();
	return 0;
}
