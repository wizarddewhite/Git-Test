#ifndef _VMA_H_
#define _VMA_H_

#include <string.h>
#include "rb_tree.h"

#define PAGE_SHIFT		12
#define	ENOMEM		12	/* Out of memory */

/*
 * The type of an index into the pagecache.
 */
#define pgoff_t unsigned long

/**
 * swap - swap values of @a and @b
 * @a: first value
 * @b: second value
 */
#define swap(a, b) \
	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)


struct mm_struct;
struct vm_area_struct {
	/* The first cache line has the info for VMA tree walking. */

	unsigned long vm_start;		/* Our start address within vm_mm. */
	unsigned long vm_end;		/* The first byte after our end address
					   within vm_mm. */

	/* linked list of VM areas per task, sorted by address */
	struct vm_area_struct *vm_next, *vm_prev;

	struct rb_node vm_rb;

	/*
	 * Largest free memory gap in bytes to the left of this VMA.
	 * Either between this VMA and vma->vm_prev, or between one of the
	 * VMAs below us in the VMA rbtree and its ->vm_prev. This helps
	 * get_unmapped_area find a free area of the right size.
	 */
	unsigned long rb_subtree_gap;

	/* Second cache line starts here. */

	struct mm_struct *vm_mm;	/* The address space we belong to. */
	//pgprot_t vm_page_prot;		/* Access permissions of this VMA. */
	unsigned long vm_flags;		/* Flags, see mm.h. */

	/*
	 * For areas with an address space and backing store,
	 * linkage into the address_space->i_mmap interval tree.
	 */
	struct {
		struct rb_node rb;
		unsigned long rb_subtree_last;
	} shared;

	/*
	 * A file's MAP_PRIVATE vma can be in both i_mmap tree and anon_vma
	 * list, after a COW of one of the file pages.	A MAP_SHARED vma
	 * can only be in the i_mmap tree.  An anonymous MAP_PRIVATE, stack
	 * or brk vma (with NULL file) can only be in an anon_vma list.
	 */
	//struct list_head anon_vma_chain; /* Serialized by mmap_sem &
	//				  * page_table_lock */
	//struct anon_vma *anon_vma;	/* Serialized by page_table_lock */

	/* Function pointers to deal with this struct. */
	//const struct vm_operations_struct *vm_ops;

	/* Information about our backing store: */
	unsigned long vm_pgoff;		/* Offset (within vm_file) in PAGE_SIZE
					   units */
	//struct file * vm_file;		/* File we map to (can be NULL). */
	void * vm_private_data;		/* was vm_pte (shared mem) */

#ifdef CONFIG_SWAP
	atomic_long_t swap_readahead_info;
#endif
#ifndef CONFIG_MMU
	struct vm_region *vm_region;	/* NOMMU mapping region */
#endif
#ifdef CONFIG_NUMA
	struct mempolicy *vm_policy;	/* NUMA policy for the VMA */
#endif
	//struct vm_userfaultfd_ctx vm_userfaultfd_ctx;
} /* __randomize_layout */;

struct mm_struct {
	struct vm_area_struct *mmap;		/* list of VMAs */
	struct rb_root mm_rb;
	unsigned long highest_vm_end;	/* highest vma end address */
	int map_count;			/* number of VMAs */
};

static inline void vma_init(struct vm_area_struct *vma, struct mm_struct *mm)
{
	//static const struct vm_operations_struct dummy_vm_ops = {};

	memset(vma, 0, sizeof(*vma));
	vma->vm_mm = mm;
	//vma->vm_ops = &dummy_vm_ops;
	//INIT_LIST_HEAD(&vma->anon_vma_chain);
}


static inline unsigned long vm_start_gap(struct vm_area_struct *vma)
{
	unsigned long vm_start = vma->vm_start;

	/* 
	if (vma->vm_flags & VM_GROWSDOWN) {
		vm_start -= stack_guard_gap;
		if (vm_start > vma->vm_start)
			vm_start = 0;
	}
	*/
	return vm_start;
}

static inline unsigned long vm_end_gap(struct vm_area_struct *vma)
{
	unsigned long vm_end = vma->vm_end;

	/* 
	if (vma->vm_flags & VM_GROWSUP) {
		vm_end += stack_guard_gap;
		if (vm_end < vma->vm_end)
			vm_end = -PAGE_SIZE;
	}
	*/
	return vm_end;
}

unsigned long subtree_compute_count;
void vma_print(struct rb_node *node, char *prefix, int level);
struct mm_struct *mm_alloc(void);
struct vm_area_struct *vm_area_alloc(struct mm_struct *mm);
void vma_link(struct mm_struct *mm, struct vm_area_struct *vma,
			struct vm_area_struct *prev, struct rb_node **rb_link,
			struct rb_node *rb_parent);
void __insert_vm_struct(struct mm_struct *mm, struct vm_area_struct *vma);
int find_vma_links(struct mm_struct *mm, unsigned long addr,
		unsigned long end, struct vm_area_struct **pprev,
		struct rb_node ***rb_link, struct rb_node **rb_parent);
struct vm_area_struct *find_vma(struct mm_struct *mm, unsigned long addr);
struct vm_area_struct *
find_vma_prev(struct mm_struct *mm, unsigned long addr,
			struct vm_area_struct **pprev);
void vma_rb_erase_ignore(struct vm_area_struct *vma,
				struct rb_root *root,
				struct vm_area_struct *ignore);
void vma_gap_update(struct vm_area_struct *vma);
void __vma_unlink_list(struct mm_struct *mm, struct vm_area_struct *vma);
#endif
