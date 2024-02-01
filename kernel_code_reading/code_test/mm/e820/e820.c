#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "e820.h"

static struct e820_table e820_table_init;
struct e820_table *e820_table = &e820_table_init;

static void e820_print_type(enum e820_type type)
{
	switch (type) {
	case E820_TYPE_RAM:		/* Fall through: */
	case E820_TYPE_RESERVED_KERN:	printf("usable");			break;
	case E820_TYPE_RESERVED:	printf("reserved");			break;
	case E820_TYPE_SOFT_RESERVED:	printf("soft reserved");		break;
	case E820_TYPE_ACPI:		printf("ACPI data");			break;
	case E820_TYPE_NVS:		printf("ACPI NVS");			break;
	case E820_TYPE_UNUSABLE:	printf("unusable");			break;
	case E820_TYPE_PMEM:		/* Fall through: */
	case E820_TYPE_PRAM:		printf("persistent (type %u)", type);	break;
	default:			printf("type %u", type);		break;
	}
}

void e820__print_table(char *who)
{
	int i;

	for (i = 0; i < e820_table->nr_entries; i++) {
		printf("%s: [mem %#018Lx-%#018Lx] ",
			who,
			e820_table->entries[i].addr,
			e820_table->entries[i].addr + e820_table->entries[i].size - 1);

		e820_print_type(e820_table->entries[i].type);
		printf("\n");
	}
}

/*
 * Add a memory region to the kernel E820 map.
 */
static void __e820__range_add(struct e820_table *table, u64 start, u64 size, enum e820_type type)
{
	int x = table->nr_entries;

	if (x >= ARRAY_SIZE(table->entries)) {
		printf("too many entries; ignoring [mem %#010llx-%#010llx]\n",
		       start, start + size - 1);
		return;
	}

	table->entries[x].addr = start;
	table->entries[x].size = size;
	table->entries[x].type = type;
	table->nr_entries++;
}

void e820__range_add(u64 start, u64 size, enum e820_type type)
{
	__e820__range_add(e820_table, start, size, type);
}

static struct change_member	change_point_list[2*E820_MAX_ENTRIES]	;
static struct change_member	*change_point[2*E820_MAX_ENTRIES]	;
static struct e820_entry	*overlap_list[E820_MAX_ENTRIES]		;
static struct e820_entry	new_entries[E820_MAX_ENTRIES]		;

static int cpcompare(const void *a, const void *b)
{
	struct change_member * const *app = a, * const *bpp = b;
	const struct change_member *ap = *app, *bp = *bpp;

	/*
	 * Inputs are pointers to two elements of change_point[].  If their
	 * addresses are not equal, their difference dominates.  If the addresses
	 * are equal, then consider one that represents the end of its region
	 * to be greater than one that does not.
	 */
	if (ap->addr != bp->addr)
		return ap->addr > bp->addr ? 1 : -1;

	return (ap->addr != ap->entry->addr) - (bp->addr != bp->entry->addr);
}

static bool e820_nomerge(enum e820_type type)
{
	/*
	 * These types may indicate distinct platform ranges aligned to
	 * numa node, protection domain, performance domain, or other
	 * boundaries. Do not merge them.
	 */
	if (type == E820_TYPE_PRAM)
		return true;
	if (type == E820_TYPE_SOFT_RESERVED)
		return true;
	return false;
}

int e820__update_table(struct e820_table *table)
{
	struct e820_entry *entries = table->entries;
	u32 max_nr_entries = ARRAY_SIZE(table->entries);
	enum e820_type current_type, last_type;
	unsigned long long last_addr;
	u32 new_nr_entries, overlap_entries;
	u32 i, chg_idx, chg_nr;

	/* If there's only one memory region, don't bother: */
	if (table->nr_entries < 2)
		return -1;

	// BUG_ON(table->nr_entries > max_nr_entries);

	/* Bail out if we find any unreasonable addresses in the map: */
	for (i = 0; i < table->nr_entries; i++) {
		if (entries[i].addr + entries[i].size < entries[i].addr)
			return -1;
	}

	/* Create pointers for initial change-point information (for sorting): */
	for (i = 0; i < 2 * table->nr_entries; i++)
		change_point[i] = &change_point_list[i];

	/*
	 * Record all known change-points (starting and ending addresses),
	 * omitting empty memory regions:
	 */
	chg_idx = 0;
	for (i = 0; i < table->nr_entries; i++)	{
		if (entries[i].size != 0) {
			change_point[chg_idx]->addr	= entries[i].addr;
			change_point[chg_idx++]->entry	= &entries[i];
			change_point[chg_idx]->addr	= entries[i].addr + entries[i].size;
			change_point[chg_idx++]->entry	= &entries[i];
		}
	}
	chg_nr = chg_idx;

	/* Sort change-point list by memory addresses (low -> high): */
	qsort(change_point, chg_nr, sizeof(*change_point), cpcompare);

	/* Create a new memory map, removing overlaps: */
	overlap_entries = 0;	 /* Number of entries in the overlap table */
	new_nr_entries = 0;	 /* Index for creating new map entries */
	last_type = 0;		 /* Start with undefined memory type */
	last_addr = 0;		 /* Start with 0 as last starting address */

	/* Loop through change-points, determining effect on the new map: */
	for (chg_idx = 0; chg_idx < chg_nr; chg_idx++) {
		/* Keep track of all overlapping entries */
		if (change_point[chg_idx]->addr == change_point[chg_idx]->entry->addr) {
			/* Add map entry to overlap list (> 1 entry implies an overlap) */
			overlap_list[overlap_entries++] = change_point[chg_idx]->entry;
		} else {
			/* Remove entry from list (order independent, so swap with last): */
			for (i = 0; i < overlap_entries; i++) {
				if (overlap_list[i] == change_point[chg_idx]->entry)
					overlap_list[i] = overlap_list[overlap_entries-1];
			}
			overlap_entries--;
		}
		/*
		 * If there are overlapping entries, decide which
		 * "type" to use (larger value takes precedence --
		 * 1=usable, 2,3,4,4+=unusable)
		 */
		current_type = 0;
		for (i = 0; i < overlap_entries; i++) {
			if (overlap_list[i]->type > current_type)
				current_type = overlap_list[i]->type;
		}

		/* Continue building up new map based on this information: */
		if (current_type != last_type || e820_nomerge(current_type)) {
			if (last_type) {
				new_entries[new_nr_entries].size = change_point[chg_idx]->addr - last_addr;
				/* Move forward only if the new size was non-zero: */
				if (new_entries[new_nr_entries].size != 0)
					/* No more space left for new entries? */
					if (++new_nr_entries >= max_nr_entries)
						break;
			}
			if (current_type) {
				new_entries[new_nr_entries].addr = change_point[chg_idx]->addr;
				new_entries[new_nr_entries].type = current_type;
				last_addr = change_point[chg_idx]->addr;
			}
			last_type = current_type;
		}
	}

	/* Copy the new entries into the original location: */
	memcpy(entries, new_entries, new_nr_entries*sizeof(*entries));
	table->nr_entries = new_nr_entries;

	return 0;
}

int e820__update_table2(struct e820_table *table)
{
	struct e820_entry *entries = table->entries;
	u32 max_nr_entries = ARRAY_SIZE(table->entries);
	enum e820_type current_type, last_type;
	u32 new_nr_entries, overlap_entries;
	u32 i, chg_idx, chg_nr;

	/* If there's only one memory region, don't bother: */
	if (table->nr_entries < 2)
		return -1;

	// BUG_ON(table->nr_entries > max_nr_entries);

	/* Bail out if we find any unreasonable addresses in the map: */
	for (i = 0; i < table->nr_entries; i++) {
		if (entries[i].addr + entries[i].size < entries[i].addr)
			return -1;
	}

	/* Create pointers for initial change-point information (for sorting): */
	for (i = 0; i < 2 * table->nr_entries; i++)
		change_point[i] = &change_point_list[i];

	/*
	 * Record all known change-points (starting and ending addresses),
	 * omitting empty memory regions:
	 */
	chg_idx = 0;
	for (i = 0; i < table->nr_entries; i++)	{
		if (entries[i].size != 0) {
			change_point[chg_idx]->addr	= entries[i].addr;
			change_point[chg_idx++]->entry	= &entries[i];
			change_point[chg_idx]->addr	= entries[i].addr + entries[i].size;
			change_point[chg_idx++]->entry	= &entries[i];
		}
	}
	chg_nr = chg_idx;

	/* Sort change-point list by memory addresses (low -> high): */
	qsort(change_point, chg_nr, sizeof(*change_point), cpcompare);

	/* Create a new memory map, removing overlaps: */
	overlap_entries = 0;	 /* Number of entries in the overlap table */
	new_nr_entries = 0;	 /* Index for creating new map entries */
	last_type = 0;		 /* Start with undefined memory type */

	/* Loop through change-points, determining effect on the new map: */
	for (chg_idx = 0; chg_idx < chg_nr; chg_idx++) {
		/* Keep track of all overlapping entries */
		if (change_point[chg_idx]->addr == change_point[chg_idx]->entry->addr) {
			/* Add map entry to overlap list (> 1 entry implies an overlap) */
			overlap_list[overlap_entries++] = change_point[chg_idx]->entry;
		} else {
			/* Remove entry from list (order independent, so swap with last): */
			for (i = 0; i < overlap_entries; i++) {
				if (overlap_list[i] == change_point[chg_idx]->entry) {
					overlap_list[i] = overlap_list[--overlap_entries];
					break;
				}
			}
		}
		/*
		 * If there are overlapping entries, decide which
		 * "type" to use (larger value takes precedence --
		 * 1=usable, 2,3,4,4+=unusable)
		 */
		current_type = 0;
		for (i = 0; i < overlap_entries; i++) {
			if (overlap_list[i]->type > current_type)
				current_type = overlap_list[i]->type;
		}

		/* Continue building up new map based on this information: */
		if (current_type != last_type || e820_nomerge(current_type)) {
			if (last_type) {
				new_entries[new_nr_entries].size = 
					change_point[chg_idx]->addr - new_entries[new_nr_entries].addr;
				/* Move forward only if the new size was non-zero: */
				if (new_entries[new_nr_entries].size != 0)
					/* No more space left for new entries? */
					if (++new_nr_entries >= max_nr_entries)
						break;
			}
			if (current_type) {
				new_entries[new_nr_entries].addr = change_point[chg_idx]->addr;
				new_entries[new_nr_entries].type = current_type;
			}
			last_type = current_type;
		}
	}

	/* Copy the new entries into the original location: */
	memcpy(entries, new_entries, new_nr_entries*sizeof(*entries));
	table->nr_entries = new_nr_entries;

	return 0;
}

static u64
__e820__range_update(struct e820_table *table, u64 start, u64 size, enum e820_type old_type, enum e820_type new_type)
{
	u64 end;
	unsigned int i;
	u64 real_updated_size = 0;

	// BUG_ON(old_type == new_type);

	if (size > (ULLONG_MAX - start))
		size = ULLONG_MAX - start;

	end = start + size;
	printf("e820: update [mem %#010Lx-%#010Lx] ", start, end - 1);
	e820_print_type(old_type);
	printf(" ==> ");
	e820_print_type(new_type);
	printf("\n");

	for (i = 0; i < table->nr_entries; i++) {
		struct e820_entry *entry = &table->entries[i];
		u64 final_start, final_end;
		u64 entry_end;

		if (entry->type != old_type)
			continue;

		entry_end = entry->addr + entry->size;

		/* Completely covered by new range? */
		if (entry->addr >= start && entry_end <= end) {
			entry->type = new_type;
			real_updated_size += entry->size;
			continue;
		}

		/* New range is completely covered? */
		if (entry->addr < start && entry_end > end) {
			__e820__range_add(table, start, size, new_type);
			__e820__range_add(table, end, entry_end - end, entry->type);
			entry->size = start - entry->addr;
			real_updated_size += size;
			continue; // break?
		}

		/* Partially covered: */
		final_start = max(start, entry->addr);
		final_end = min(end, entry_end);
		if (final_start >= final_end)
			continue;

		__e820__range_add(table, final_start, final_end - final_start, new_type);

		real_updated_size += final_end - final_start;

		/*
		 * Left range could be head or tail, so need to update
		 * its size first:
		 */
		entry->size -= final_end - final_start;
		if (entry->addr < final_start)
			continue;

		entry->addr = final_end;
	}
	return real_updated_size;
}

u64 e820__range_update(u64 start, u64 size, enum e820_type old_type, enum e820_type new_type)
{
	return __e820__range_update(e820_table, start, size, old_type, new_type);
}

static u64
__e820__range_update2(struct e820_table *table, u64 start, u64 size, enum e820_type old_type, enum e820_type new_type)
{
	u64 end;
	unsigned int i;
	u64 real_updated_size = 0;

	// BUG_ON(old_type == new_type);

	if (size > (ULLONG_MAX - start))
		size = ULLONG_MAX - start;

	end = start + size;
	printf("e820: update [mem %#010Lx-%#010Lx] ", start, end - 1);
	e820_print_type(old_type);
	printf(" ==> ");
	e820_print_type(new_type);
	printf("\n");

	for (i = 0; i < table->nr_entries; i++) {
		struct e820_entry *entry = &table->entries[i];
		u64 final_start, final_end;
		u64 entry_end;

		if (entry->type != old_type)
			continue;

		entry_end = entry->addr + entry->size;

		/* Completely covered by new range? */
		if (entry->addr >= start && entry_end <= end) {
			entry->type = new_type;
			real_updated_size += entry->size;
			continue;
		}

		/* New range is completely covered? */
		if (entry->addr < start && entry_end > end) {
			__e820__range_add(table, start, size, new_type);
			__e820__range_add(table, end, entry_end - end, entry->type);
			entry->size = start - entry->addr;
			real_updated_size += size;
			continue; // break?
		}

		/* Partially covered: */
		final_start = max(start, entry->addr);
		final_end = min(end, entry_end);
		if (final_start >= final_end)
			continue;

		__e820__range_add(table, final_start, final_end - final_start, new_type);

		real_updated_size += final_end - final_start;

		/*
		 * Left range could be head or tail, so need to update
		 * its size first:
		 */
		entry->size -= final_end - final_start;
		if (entry->addr < final_start)
			continue;

		entry->addr = final_end;
	}
	return real_updated_size;
}

u64 e820__range_update2(u64 start, u64 size, enum e820_type old_type, enum e820_type new_type)
{
	return __e820__range_update2(e820_table, start, size, old_type, new_type);
}
