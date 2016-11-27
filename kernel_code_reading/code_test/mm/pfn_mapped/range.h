#ifndef _RANGE_H
#define _RANGE_H

typedef unsigned long long u64;

struct range {
	u64   start;
	u64   end;
};

#define E820_X_MAX	128		/* number of entries in E820MAP */
extern struct range pfn_mapped[E820_X_MAX];
extern int nr_pfn_mapped;
void dump_pfn_mapped();
#endif // _RANGE_H

