#ifndef RESV_MAP_H
#define RESV_MAP_H

#include "list.h"

struct resv_map {
	// struct kref refs;
	// spinlock_t lock;
	struct list_head regions;
	long adds_in_progress;
	struct list_head region_cache;
	long region_cache_count;
};

struct file_region {
	struct list_head link;
	long from;
	long to;
};
#endif
