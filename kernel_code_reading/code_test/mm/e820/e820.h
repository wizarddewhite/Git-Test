#ifndef E820_H
#define E820_H

#define E820MAX	128		/* number of entries in E820MAP */
#define E820_MAX_ENTRIES E820MAX

typedef	unsigned long long	__u64;
typedef	unsigned int		__u32;
typedef	unsigned long long	u64;
typedef	unsigned int		u32;

#define ULLONG_MAX	(~0ULL)

# define PAGE_SHIFT		12
# define MAX_PHYSMEM_BITS	46
# define MAXMEM			(1LL << MAX_PHYSMEM_BITS)
# define MAX_ARCH_PFN 		(MAXMEM >> PAGE_SHIFT)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define max(a,b) ((a) > (b) ? a : b)
#define min(a,b) ((a) < (b) ? a : b)

/*
 * These are the E820 types known to the kernel:
 */
enum e820_type {
	E820_TYPE_RAM		= 1,
	E820_TYPE_RESERVED	= 2,
	E820_TYPE_ACPI		= 3,
	E820_TYPE_NVS		= 4,
	E820_TYPE_UNUSABLE	= 5,
	E820_TYPE_PMEM		= 7,

	/*
	 * This is a non-standardized way to represent ADR or
	 * NVDIMM regions that persist over a reboot.
	 *
	 * The kernel will ignore their special capabilities
	 * unless the CONFIG_X86_PMEM_LEGACY=y option is set.
	 *
	 * ( Note that older platforms also used 6 for the same
	 *   type of memory, but newer versions switched to 12 as
	 *   6 was assigned differently. Some time they will learn... )
	 */
	E820_TYPE_PRAM		= 12,

	/*
	 * Special-purpose memory is indicated to the system via the
	 * EFI_MEMORY_SP attribute. Define an e820 translation of this
	 * memory type for the purpose of reserving this range and
	 * marking it with the IORES_DESC_SOFT_RESERVED designation.
	 */
	E820_TYPE_SOFT_RESERVED	= 0xefffffff,

	/*
	 * Reserved RAM used by the kernel itself if
	 * CONFIG_INTEL_TXT=y is enabled, memory of this type
	 * will be included in the S3 integrity calculation
	 * and so should not include any memory that the BIOS
	 * might alter over the S3 transition:
	 */
	E820_TYPE_RESERVED_KERN	= 128,
};

struct e820_entry {
	u64 		addr;	/* start of memory segment */
	u64 		size;	/* size of memory segment */
	enum e820_type 	type;	/* type of memory segment */
} __attribute__((packed));

/*
 * The whole array of E820 entries:
 */
struct e820_table {
	__u32 nr_entries;
	struct e820_entry entries[E820_MAX_ENTRIES];
};

/*
 * Sanitize the BIOS e820 map.
 *
 * Some e820 responses include overlapping entries. The following
 * replaces the original e820 map with a new one, removing overlaps,
 * and resolving conflicting memory types in favor of highest
 * numbered type.
 *
 * The input parameter biosmap points to an array of 'struct
 * e820entry' which on entry has elements in the range [0, *pnr_map)
 * valid, and which has space for up to max_nr_map entries.
 * On return, the resulting sanitized e820 map entries will be in
 * overwritten in the same location, starting at biosmap.
 *
 * The integer pointed to by pnr_map must be valid on entry (the
 * current number of valid entries located at biosmap). If the
 * sanitizing succeeds the *pnr_map will be updated with the new
 * number of valid entries (something no more than max_nr_map).
 *
 * The return value from sanitize_e820_map() is zero if it
 * successfully 'sanitized' the map entries passed in, and is -1
 * if it did nothing, which can happen if either of (1) it was
 * only passed one map entry, or (2) any of the input map entries
 * were invalid (start + size < start, meaning that the size was
 * so big the described memory range wrapped around through zero.)
 *
 *	Visually we're performing the following
 *	(1,2,3,4 = memory types)...
 *
 *	Sample memory map (w/overlaps):
 *	   ____22__________________
 *	   ______________________4_
 *	   ____1111________________
 *	   _44_____________________
 *	   11111111________________
 *	   ____________________33__
 *	   ___________44___________
 *	   __________33333_________
 *	   ______________22________
 *	   ___________________2222_
 *	   _________111111111______
 *	   _____________________11_
 *	   _________________4______
 *
 *	Sanitized equivalent (no overlap):
 *	   1_______________________
 *	   _44_____________________
 *	   ___1____________________
 *	   ____22__________________
 *	   ______11________________
 *	   _________1______________
 *	   __________3_____________
 *	   ___________44___________
 *	   _____________33_________
 *	   _______________2________
 *	   ________________1_______
 *	   _________________4______
 *	   ___________________2____
 *	   ____________________33__
 *	   ______________________4_
 */
struct change_member {
	struct e820entry *pbios; /* pointer to original bios entry */
	unsigned long long addr; /* address for this change point */
};

void e820__range_add(u64 start, u64 size, enum e820_type type);
void e820__print_table(char *who);
#endif //E820_H
