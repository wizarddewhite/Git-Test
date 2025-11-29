#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define BIT_ULL(nr)                   (1ULL << (nr))
#define PM_SOFT_DIRTY                 BIT_ULL(55)
#define PM_MMAP_EXCLUSIVE             BIT_ULL(56)
#define PM_UFFD_WP                    BIT_ULL(57)
#define PM_GUARD_REGION               BIT_ULL(58)
#define PM_FILE                       BIT_ULL(61)
#define PM_SWAP                       BIT_ULL(62)
#define PM_PRESENT                    BIT_ULL(63)

struct pagemap_info {
	char *addr;
	unsigned long pfn;
	bool is_file;
};

uint64_t pagemap_get_entry(char *start);
unsigned long pagemap_get_pfn(char *start);
void pagemap_get_info(struct pagemap_info *info);
