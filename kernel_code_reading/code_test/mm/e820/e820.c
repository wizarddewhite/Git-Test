#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e820.h"

struct e820map bootmap;
struct e820map e820;
struct e820map e820_saved;

static void e820_print_type(__u32 type)
{
	switch (type) {
	case E820_RAM:
	case E820_RESERVED_KERN:
		printf("usable");
		break;
	case E820_RESERVED:
		printf("reserved");
		break;
	case E820_ACPI:
		printf("ACPI data");
		break;
	case E820_NVS:
		printf("ACPI NVS");
		break;
	case E820_UNUSABLE:
		printf("unusable");
		break;
	case E820_PMEM:
	case E820_PRAM:
		printf("persistent (type %u)", type);
		break;
	default:
		printf("type %u", type);
		break;
	}
}

void e820_print_map(char *who, struct e820map *e820)
{
	int i;

	for (i = 0; i < e820->nr_map; i++) {
		printf("%s: [mem %#018Lx-%#018Lx] ", who,
		       (unsigned long long) e820->map[i].addr,
		       (unsigned long long)
		       (e820->map[i].addr + e820->map[i].size - 1));
		e820_print_type(e820->map[i].type);
		printf("\n");
	}
}

static int cpcompare(const void *a, const void *b)
{
	struct change_member * const *app = a, * const *bpp = b;
	const struct change_member *ap = *app, *bp = *bpp;

	/*
	 * Inputs are pointers to two elements of change_point[].  If their
	 * addresses are unequal, their difference dominates.  If the addresses
	 * are equal, then consider one that represents the end of its region
	 * to be greater than one that does not.
	 */
	if (ap->addr != bp->addr)
		return ap->addr > bp->addr ? 1 : -1;

	return (ap->addr != ap->pbios->addr) - (bp->addr != bp->pbios->addr);
}

int sanitize_e820_map(struct e820entry *biosmap, int max_nr_map,
			     u32 *pnr_map)
{
	static struct change_member change_point_list[2*E820_X_MAX] ;
	static struct change_member *change_point[2*E820_X_MAX] ;
	static struct e820entry *overlap_list[E820_X_MAX] ;
	static struct e820entry new_bios[E820_X_MAX] ;
	unsigned long current_type, last_type;
	unsigned long long last_addr;
	int chgidx;
	int overlap_entries;
	int new_bios_entry;
	int old_nr, new_nr, chg_nr;
	int i;

	/* if there's only one memory region, don't bother */
	if (*pnr_map < 2)
		return -1;

	old_nr = *pnr_map;
	if(old_nr > max_nr_map) {
		printf("%s: old_nr > max_nr_map\n", __func__);
		exit(-1);
	}

	/* bail out if we find any unreasonable addresses in bios map */
	for (i = 0; i < old_nr; i++)
		if (biosmap[i].addr + biosmap[i].size < biosmap[i].addr)
			return -1;

	/* create pointers for initial change-point information (for sorting) */
	for (i = 0; i < 2 * old_nr; i++)
		change_point[i] = &change_point_list[i];

	/* record all known change-points (starting and ending addresses),
	   omitting those that are for empty memory regions */
	chgidx = 0;
	for (i = 0; i < old_nr; i++)	{
		if (biosmap[i].size != 0) {
			change_point[chgidx]->addr = biosmap[i].addr;
			change_point[chgidx++]->pbios = &biosmap[i];
			change_point[chgidx]->addr = biosmap[i].addr +
				biosmap[i].size;
			change_point[chgidx++]->pbios = &biosmap[i];
		}
	}
	chg_nr = chgidx;

	/* sort change-point list by memory addresses (low -> high) */
	qsort(change_point, chg_nr, sizeof *change_point, cpcompare);

	/* create a new bios memory map, removing overlaps */
	overlap_entries = 0;	 /* number of entries in the overlap table */
	new_bios_entry = 0;	 /* index for creating new bios map entries */
	last_type = 0;		 /* start with undefined memory type */
	last_addr = 0;		 /* start with 0 as last starting address */

	/* loop through change-points, determining affect on the new bios map */
	for (chgidx = 0; chgidx < chg_nr; chgidx++) {
		/* keep track of all overlapping bios entries */
		if (change_point[chgidx]->addr ==
		    change_point[chgidx]->pbios->addr) {
			/*
			 * add map entry to overlap list (> 1 entry
			 * implies an overlap)
			 */
			overlap_list[overlap_entries++] =
				change_point[chgidx]->pbios;
		} else {
			/*
			 * remove entry from list (order independent,
			 * so swap with last)
			 */
			for (i = 0; i < overlap_entries; i++) {
				if (overlap_list[i] ==
				    change_point[chgidx]->pbios)
					overlap_list[i] =
						overlap_list[overlap_entries-1];
			}
			overlap_entries--;
		}
		/*
		 * if there are overlapping entries, decide which
		 * "type" to use (larger value takes precedence --
		 * 1=usable, 2,3,4,4+=unusable)
		 */
		current_type = 0;
		for (i = 0; i < overlap_entries; i++)
			if (overlap_list[i]->type > current_type)
				current_type = overlap_list[i]->type;
		/*
		 * continue building up new bios map based on this
		 * information
		 */
		if (current_type != last_type || current_type == E820_PRAM) {
			if (last_type != 0)	 {
				new_bios[new_bios_entry].size =
					change_point[chgidx]->addr - last_addr;
				/*
				 * move forward only if the new size
				 * was non-zero
				 */
				if (new_bios[new_bios_entry].size != 0)
					/*
					 * no more space left for new
					 * bios entries ?
					 */
					if (++new_bios_entry >= max_nr_map)
						break;
			}
			if (current_type != 0)	{
				new_bios[new_bios_entry].addr =
					change_point[chgidx]->addr;
				new_bios[new_bios_entry].type = current_type;
				last_addr = change_point[chgidx]->addr;
			}
			last_type = current_type;
		}
	}
	/* retain count for new bios entries */
	new_nr = new_bios_entry;

	/* copy new bios mapping into original location */
	memcpy(biosmap, new_bios, new_nr * sizeof(struct e820entry));
	*pnr_map = new_nr;

	return 0;
}

static void __e820_add_region(struct e820map *e820x, u64 start, u64 size,
					 int type)
{
	int x = e820x->nr_map;

	if (x >= ARRAY_SIZE(e820x->map)) {
		printf("e820: too many entries; ignoring [mem %#010llx-%#010llx]\n",
		       (unsigned long long) start,
		       (unsigned long long) (start + size - 1));
		return;
	}

	e820x->map[x].addr = start;
	e820x->map[x].size = size;
	e820x->map[x].type = type;
	e820x->nr_map++;
}

void e820_add_region(u64 start, u64 size, int type)
{
	__e820_add_region(&e820, start, size, type);
}

static int __append_e820_map(struct e820entry *biosmap, int nr_map)
{
	while (nr_map) {
		u64 start = biosmap->addr;
		u64 size = biosmap->size;
		u64 end = start + size - 1;
		u32 type = biosmap->type;

		/* Overflow in 64 bits? Ignore the memory map. */
		if (start > end && (size))
			return -1;

		e820_add_region(start, size, type);

		biosmap++;
		nr_map--;
	}
	return 0;
}

int append_e820_map(struct e820entry *biosmap, int nr_map)
{
	/* Only one memory region (or negative)? Ignore it */
	if (nr_map < 2)
		return -1;

	return __append_e820_map(biosmap, nr_map);
}
