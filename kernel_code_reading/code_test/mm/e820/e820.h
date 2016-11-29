#ifndef E820_H
#define E820_H

#define E820MAX	128		/* number of entries in E820MAP */
#define E820_X_MAX E820MAX

#define E820_RAM	1
#define E820_RESERVED	2
#define E820_ACPI	3
#define E820_NVS	4
#define E820_UNUSABLE	5
#define E820_PMEM	7

#define E820_PRAM	12

#define E820_RESERVED_KERN        128

typedef	unsigned long long	__u64;
typedef	unsigned int		__u32;

struct e820entry {
	__u64 addr;	/* start of memory segment */
	__u64 size;	/* size of memory segment */
	__u32 type;	/* type of memory segment */
} __attribute__((packed));

struct e820map {
	__u32 nr_map;
	struct e820entry map[E820_X_MAX];
};

extern struct e820map bootmap;
extern struct e820map e820;
extern struct e820map e820_saved;

#endif //E820_H
