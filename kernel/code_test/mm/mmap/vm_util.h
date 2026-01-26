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

#define KPF_COMPOUND_HEAD             BIT_ULL(15)
#define KPF_COMPOUND_TAIL             BIT_ULL(16)
#define KPF_THP                       BIT_ULL(22)

struct pagemap_info {
	char *addr;
	unsigned long pfn;
	bool is_file;
};

uint64_t pagemap_get_entry(char *start);
unsigned long pagemap_get_pfn(char *start);
void pagemap_get_info(struct pagemap_info *info);
uint64_t read_pmd_pagesize(void);
bool check_huge_anon(void *addr, int nr_hpages, uint64_t hpage_size);
bool check_anon(void *addr, int nr_hpages, uint64_t page_size);
uint64_t get_huge_anon(void *addr);
uint64_t get_anon(void *addr);
int pageflags_get(unsigned long pfn, int kpageflags_fd, uint64_t *flags);
