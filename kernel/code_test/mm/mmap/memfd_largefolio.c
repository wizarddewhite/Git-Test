/*
 * memfd_largefolio.c
 *
 * 目的：用 memfd_create() 造一个"匿名文件"（内部 shmem mount），把它 mmap 到
 *       用户态，然后验证底层是否真的分配到了 large folio（mTHP / PMD THP），
 *       而不是一堆 order-0 的 4K 页。
 *
 * 内核机制背景（v6.x / v7.x）：
 *   memfd_create() -> shmem_file_setup()，落在"内部 shmem mount"（shm_mnt）上。
 *   SysV SHM、MAP_ANONYMOUS|MAP_SHARED、DRM/Ashmem 也走这个 mount。
 *   缺页路径 shmem_fault() -> shmem_get_folio_gfp() -> shmem_alloc_and_add_folio()，
 *   其中：
 *     - vma_is_anon_shmem(vma) 为真时（memfd 就是），走 shmem_suitable_orders()，
 *       可以返回任意阶的 order（受 per-size sysfs 控制），这是拿到 mTHP 的关键；
 *     - 普通 tmpfs 只有 vma 路径拿 PMD 阶，write/fallocate 路径才能拿任意阶。
 *   order 决策链：
 *     shmem_allowable_huge_orders()  <- huge_shmem_orders_{always,within_size,madvise,inherit}
 *                                       来自 /sys/kernel/mm/transparent_hugepage/
 *                                            shmem_enabled                (顶层)
 *                                            hugepages-<N>kB/shmem_enabled (per-size)
 *     shmem_suitable_orders()        <- thp_vma_suitable_orders() 检查地址对齐 + VMA 范围
 *                                    <- xarray 检查目标范围没有已存在的小页冲突
 *
 *   默认值很坑：顶层 shmem_enabled 默认 never；per-size 只有 PMD 阶是 inherit，
 *   其余全部 never。所以不做任何配置的话，memfd 只会拿到 4K 页。
 *
 * 编译：gcc -O2 -Wall -o memfd_largefolio memfd_largefolio.c
 *
 * 典型用法（需要 root 才能改 sysfs 和读 /proc/kpageflags）：
 *   # 方式 A：让程序自己配 sysfs
 *   sudo ./memfd_largefolio -s 64M -O 4 -m fault -S -p always
 *   sudo ./memfd_largefolio -s 64M -O 9 -m collapse -S -p always
 *
 *   # 方式 B：自己先配好，程序只做验证
 *   echo always | sudo tee /sys/kernel/mm/transparent_hugepage/shmem_enabled
 *   echo always | sudo tee /sys/kernel/mm/transparent_hugepage/hugepages-64kB/shmem_enabled
 *   sudo ./memfd_largefolio -s 64M -O 4 -m fault
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#ifndef MFD_CLOEXEC
#define MFD_CLOEXEC 0x0001U
#endif
#ifndef MFD_ALLOW_SEALING
#define MFD_ALLOW_SEALING 0x0002U
#endif
#ifndef MADV_HUGEPAGE
#define MADV_HUGEPAGE 14
#endif
#ifndef MADV_COLLAPSE
#define MADV_COLLAPSE 25
#endif
#ifndef MADV_POPULATE_WRITE
#define MADV_POPULATE_WRITE 23
#endif

/* /proc/kpageflags 的位定义 */
#define KPF_COMPOUND_HEAD 15
#define KPF_COMPOUND_TAIL 16
#define KPF_THP           22

#define SYSFS_THP "/sys/kernel/mm/transparent_hugepage"

static size_t page_size;
static size_t pmd_size;
static int    pmd_order;
static int    verbose = 1;

/* ------------------------------------------------------------------ */
/* 小工具                                                              */
/* ------------------------------------------------------------------ */

static int read_sysfs_str(const char *path, char *buf, size_t buflen)
{
	int fd = open(path, O_RDONLY);
	ssize_t n;
	if (fd < 0)
		return -1;
	n = read(fd, buf, buflen - 1);
	close(fd);
	if (n < 0)
		return -1;
	buf[n] = '\0';
	while (n > 0 && (buf[n - 1] == '\n' || buf[n - 1] == ' '))
		buf[--n] = '\0';
	return 0;
}

/* 取 sysfs 里被 [] 包起来的当前值，例如 "always [madvise] never" -> "madvise" */
static int read_sysfs_cur(const char *path, char *buf, size_t buflen)
{
	char raw[512];
	char *l, *r;
	if (read_sysfs_str(path, raw, sizeof(raw)))
		return -1;
	l = strchr(raw, '[');
	r = strchr(raw, ']');
	if (!l || !r || r < l)
		return -1;
	*r = '\0';
	snprintf(buf, buflen, "%s", l + 1);
	return 0;
}

static int write_sysfs(const char *path, const char *val)
{
	int fd = open(path, O_WRONLY);
	ssize_t n;
	if (fd < 0)
		return -1;
	n = write(fd, val, strlen(val));
	close(fd);
	return n < 0 ? -1 : 0;
}

static unsigned long read_sysfs_ul(const char *path)
{
	char buf[64] = "0";
	if (read_sysfs_str(path, buf, sizeof(buf)))
		return 0;
	return strtoul(buf, NULL, 0);
}

static int ilog2_ul(unsigned long v)
{
	int o = 0;
	while (v > 1) {
		v >>= 1;
		o++;
	}
	return o;
}

static unsigned long meminfo_field(const char *key)
{
	FILE *f;
	char line[512];
	unsigned long val = 0;

	f = fopen("/proc/meminfo", "r");
	if (!f)
		return 0;
	while (fgets(line, sizeof(line), f)) {
		if (!strncmp(line, key, strlen(key))) {
			sscanf(line + strlen(key), " %lu", &val);
			break;
		}
	}
	fclose(f);
	return val;
}

/* ------------------------------------------------------------------ */
/* 环境探测                                                            */
/* ------------------------------------------------------------------ */

static void detect_env(void)
{
	char buf[256];
	unsigned long v;

	page_size = sysconf(_SC_PAGESIZE);

	v = read_sysfs_ul(SYSFS_THP "/hpage_pmd_size");
	pmd_size = v ? v : (2UL << 20);
	pmd_order = ilog2_ul(pmd_size / page_size);

	printf("=== 环境 ===\n");
	printf("page size          : %zu B\n", page_size);
	printf("PMD (THP) size     : %zu B (order %d)\n", pmd_size, pmd_order);

	if (read_sysfs_cur(SYSFS_THP "/enabled", buf, sizeof(buf)) == 0)
		printf("THP enabled        : %s\n", buf);
	else
		printf("THP enabled        : <不可读，CONFIG_TRANSPARENT_HUGEPAGE 可能未开>\n");

	if (read_sysfs_cur(SYSFS_THP "/shmem_enabled", buf, sizeof(buf)) == 0) {
		printf("shmem_enabled(顶层) : %s   <-- deny/never 会让一切失效\n", buf);
		if (!strcmp(buf, "deny") || !strcmp(buf, "never")) {
			printf("\n\tshmem_enabled should not be deny/never\n");
			exit(1);
		}
	} else {
		printf("shmem_enabled(顶层) : <不存在，内核过旧或无 THP>\n");
		exit(1);
	}

	if (access(SYSFS_THP "/hugepages-64kB", F_OK) == 0)
		printf("per-size sysfs     : 存在（支持 mTHP 精细控制）\n");
	else
		printf("per-size sysfs     : 不存在（无 mTHP per-size 控制，只有 PMD 阶）\n");
	printf("\n");
}

/* 打印目标 order 的 per-size 策略现状 */
static void show_per_size(int order)
{
	char path[512], buf[256], st[512];
	size_t sz = page_size << order;

	snprintf(path, sizeof(path), SYSFS_THP "/hugepages-%zukB/shmem_enabled", sz / 1024);
	if (read_sysfs_cur(path, buf, sizeof(buf)) == 0) {
		printf("hugepages-%zukB/shmem_enabled : %s\n", sz / 1024, buf);
	} else {
		printf("hugepages-%zukB/shmem_enabled : <不可读/不存在>\n", sz / 1024);
	}

	snprintf(st, sizeof(st), SYSFS_THP "/hugepages-%zukB/stats/nr_shmem", sz / 1024);
	if (access(st, F_OK) == 0)
		printf("hugepages-%zukB/stats/nr_shmem : %lu\n", sz / 1024, read_sysfs_ul(st));
}

/* ------------------------------------------------------------------ */
/* sysfs 自动配置                                                      */
/* ------------------------------------------------------------------ */

/*
 * 要让 memfd 的缺页路径分配 order-N 的 folio，需要：
 *   1) 顶层 shmem_enabled 不能是 deny；advise 时还需要 madvise(MADV_HUGEPAGE)
 *   2) hugepages-<size>kB/shmem_enabled 设为 always / within_size / advise
 *      （within_size 要求 folio 完全落在 i_size 内，advise 要求 VM_HUGEPAGE）
 *   3) mmap 地址按该 order 对齐（我们用 PMD 对齐，满足所有 <= PMD 的阶）
 */
static int setup_sysfs(int order, const char *policy)
{
	char path[512], buf[256];
	size_t sz = page_size << order;
	int rc = 0;

	snprintf(path, sizeof(path), SYSFS_THP "/shmem_enabled");
	if (read_sysfs_cur(path, buf, sizeof(buf)) == 0 && !strcmp(buf, "deny")) {
		fprintf(stderr, "顶层 shmem_enabled=deny，per-size 设置会被忽略\n");
		return -1;
	}
	if (read_sysfs_cur(path, buf, sizeof(buf)) == 0 && !strcmp(buf, "force")) {
		printf("[setup] 顶层已是 force，强制 PMD 阶，跳过 per-size 设置\n");
		return 0;
	}

	snprintf(path, sizeof(path), SYSFS_THP "/hugepages-%zukB/shmem_enabled", sz / 1024);
	if (access(path, F_OK) != 0) {
		fprintf(stderr, "[setup] %s 不存在：该内核没有 mTHP per-size 接口，"
				"只能用 PMD(order %d) 或 MADV_COLLAPSE\n", path, pmd_order);
		return -1;
	}
	if (write_sysfs(path, policy)) {
		fprintf(stderr, "[setup] 写 %s = %s 失败: %s (需要 root?)\n",
			path, policy, strerror(errno));
		rc = -1;
	} else {
		printf("[setup] %s = %s\n", path, policy);
	}

	if (!strcmp(policy, "advise") || !strcmp(policy, "within_size")) {
		snprintf(path, sizeof(path), SYSFS_THP "/shmem_enabled");
		if (write_sysfs(path, "advise") == 0)
			printf("[setup] %s = advise\n", path);
	}
	return rc;
}

/* ------------------------------------------------------------------ */
/* 建立 memfd + 对齐映射                                               */
/* ------------------------------------------------------------------ */

/*
 * 关键：把文件偏移 0 映射到一个 PMD 对齐的虚拟地址上。
 * 否则 thp_vma_suitable_orders() 会因地址不对齐把高阶 order 全部裁掉。
 */
static void *map_aligned(int fd, size_t len)
{
	void *reserve, *ret;
	uintptr_t aligned;

	reserve = mmap(NULL, len + pmd_size, PROT_NONE,
		       MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
	if (reserve == MAP_FAILED)
		return MAP_FAILED;

	aligned = ((uintptr_t)reserve + pmd_size - 1) & ~(uintptr_t)(pmd_size - 1);
	ret = mmap((void *)aligned, len, PROT_READ | PROT_WRITE,
		   MAP_SHARED | MAP_FIXED, fd, 0);
	if (ret == MAP_FAILED)
		return MAP_FAILED;
	return ret;
}

/* ------------------------------------------------------------------ */
/* 触发分配                                                            */
/* ------------------------------------------------------------------ */

/* fault 模式：逐 4K 页写，走 shmem_fault() -> shmem_suitable_orders() */
static void populate_by_fault(char *p, size_t len)
{
	size_t i;
	for (i = 0; i < len; i += page_size)
		p[i] = 0x5a;
}

/* write 模式：直接 write() 到 memfd，走 shmem write 路径（无 vma，
   用 THP_ORDERS_ALL_FILE_DEFAULT + within_size 过滤），可拿到非 PMD 阶 */
static void populate_by_write(int fd, size_t len)
{
	char *buf = malloc(page_size);
	size_t off = 0;

	memset(buf, 0xa5, page_size);
	while (off < len) {
		ssize_t n = pwrite(fd, buf, page_size, off);
		if (n <= 0)
			break;
		off += n;
	}
	free(buf);
}

/* ------------------------------------------------------------------ */
/* 验证：pagemap + kpageflags 统计 folio order 分布                     */
/* ------------------------------------------------------------------ */

static int pm_fd = -1, kf_fd = -1, kf_usable = 0;

static void verify_open(void)
{
	pm_fd = open("/proc/self/pagemap", O_RDONLY);
	kf_fd = open("/proc/kpageflags", O_RDONLY);
	kf_usable = (pm_fd >= 0 && kf_fd >= 0);
	if (!kf_usable)
		return;
	/* 探一下能不能真读到内容（需要 root / CAP_SYS_ADMIN） */
	uint64_t probe = 0;
	if (pread(kf_fd, &probe, 8, 0) < 0 || probe == 0)
		kf_usable = 0;
}

static uint64_t virt_to_pfn(uintptr_t va)
{
	uint64_t entry = 0;
	off_t off = (va / page_size) * 8;

	if (pread(pm_fd, &entry, 8, off) != 8)
		return 0;
	if (!(entry & (1ULL << 63)))	/* !PRESENT */
		return 0;
	return entry & ((1ULL << 55) - 1);
}

static uint64_t pfn_flags(uint64_t pfn)
{
	uint64_t f = 0;
	if (pread(kf_fd, &f, 8, pfn * 8) != 8)
		return 0;
	return f;
}

static unsigned long hist[32];
static unsigned long nr_thp, nr_notpresent;

static void hist_flush(unsigned long pages)
{
	if (!pages)
		return;
	if (pages == 1)
		hist[0]++;
	else
		hist[ilog2_ul(pages)]++;
}

/*
 * 遍历映射的每一 4K 页：head 标记一个 folio 开始，后续 tail 计数。
 * folio 页数 = 1 + tails，order = ilog2(页数)。
 */
static int verify_folios(uintptr_t va, size_t len, const char *tag)
{
	uintptr_t a;
	unsigned long cur = 0;
	unsigned long total_folios = 0, large_folios = 0, large_pages = 0;
	int order;

	memset(hist, 0, sizeof(hist));
	nr_thp = nr_notpresent = 0;

	for (a = va; a < va + len; a += page_size) {
		uint64_t pfn = virt_to_pfn(a);
		uint64_t fl;

		if (!pfn) {
			hist_flush(cur);
			cur = 0;
			nr_notpresent++;
			continue;
		}
		fl = pfn_flags(pfn);
		if (fl & (1ULL << KPF_THP))
			nr_thp++;

		if (fl & (1ULL << KPF_COMPOUND_HEAD)) {
			hist_flush(cur);
			cur = 1;
		} else if (fl & (1ULL << KPF_COMPOUND_TAIL)) {
			cur++;
		} else {
			hist_flush(cur);
			hist[0]++;
			cur = 0;
		}
	}
	hist_flush(cur);

	printf("=== [%s] folio order 分布 ===\n", tag);
	for (order = 0; order < 32; order++) {
		if (!hist[order])
			continue;
		total_folios += hist[order];
		if (order > 0) {
			large_folios += hist[order];
			large_pages += hist[order] * (1UL << order);
		}
		printf("  order %2d (%8zu B): %6lu 个 folio, 覆盖 %10lu 页\n",
		       order, page_size << order, hist[order],
		       hist[order] * (1UL << order));
	}
	printf("  --------\n");
	printf("  folio 总数        : %lu\n", total_folios);
	printf("  large folio 数    : %lu\n", large_folios);
	printf("  large folio 覆盖  : %lu / %zu 页 (%.1f%%)\n",
	       large_pages, len / page_size,
	       len ? 100.0 * large_pages / (len / page_size) : 0.0);
	printf("  KPF_THP 页        : %lu (PMD 阶 THP)\n", nr_thp);
	if (nr_notpresent)
		printf("  未 resident 页    : %lu\n", nr_notpresent);
	printf("\n");

	return large_folios > 0;
}

/* 读 /proc/self/smaps 中该 VMA 的字段（不需要 root 的补充证据） */
static void show_smaps(uintptr_t va, const char *tag)
{
	FILE *f;
	char line[512];
	unsigned long s, e;
	int in_vma = 0;
	const char *keys[] = { "ShmemPmdMapped:", "ShmemHugePages:", "FilePmdMapped:",
			       "AnonHugePages:", "THPeligible:" };

	f = fopen("/proc/self/smaps", "r");
	if (!f)
		return;
	printf("=== [%s] smaps 中该 VMA ===\n", tag);
	while (fgets(line, sizeof(line), f)) {
		if (sscanf(line, "%lx-%lx", &s, &e) == 2) {
			in_vma = (va >= s && va < e);
			continue;
		}
		if (in_vma) {
			size_t i;
			for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
				if (!strncmp(line, keys[i], strlen(keys[i])))
					printf("  %s", line);
		}
	}
	fclose(f);
	printf("\n");
}

/* ------------------------------------------------------------------ */

static void usage(const char *p)
{
	fprintf(stderr,
	"用法: %s [选项]\n"
	"  -s SIZE     映射总大小，支持 64K/2M/64M 后缀 (默认 32M)\n"
	"  -O ORDER    目标 folio order，-1 表示 PMD 阶 (默认 PMD 阶)\n"
	"  -m MODE     fault   : 缺页路径分配 (默认)\n"
	"              write   : write() 到 memfd 路径分配\n"
	"              collapse: 先 fault 再 MADV_COLLAPSE 折叠到 PMD\n"
	"  -S          自动配置 per-size sysfs (需要 root)\n"
	"  -p POLICY   -S 时写入的策略: always|within_size|advise|never (默认 always)\n"
	"  -P          结束后暂停，方便外部用 page-types 观察\n"
	"  -q          安静模式\n"
	"\n"
	"示例:\n"
	"  # 拿 64KB (order 4) 的 large folio\n"
	"  sudo %s -s 64M -O 4 -m fault -S -p always\n"
	"  # 拿 2MB (PMD) 的 large folio，用 collapse 强制\n"
	"  sudo %s -s 64M -O 9 -m collapse\n", p, p, p);
}

static size_t parse_size(const char *s)
{
	char *end;
	unsigned long long v = strtoull(s, &end, 0);
	if (end == s)
		return 0;
	if (*end == 'K' || *end == 'k')
		v <<= 10;
	else if (*end == 'M' || *end == 'm')
		v <<= 20;
	else if (*end == 'G' || *end == 'g')
		v <<= 30;
	return (size_t)v;
}

int main(int argc, char **argv)
{
	size_t total = 32UL << 20;
	int order = -1, do_setup = 0, park = 0;
	const char *mode = "fault", *policy = "always";
	int opt;

	while ((opt = getopt(argc, argv, "s:O:m:p:SPqh")) != -1) {
		switch (opt) {
		case 's': total = parse_size(optarg); break;
		case 'O': order = atoi(optarg); break;
		case 'm': mode = optarg; break;
		case 'p': policy = optarg; break;
		case 'S': do_setup = 1; break;
		case 'P': park = 1; break;
		case 'q': verbose = 0; break;
		default:  usage(argv[0]); return 1;
		}
	}

	if (geteuid() != 0) {
		printf("Run it as root!\n");
		exit(1);
	}

	detect_env();
	if (order < 0)
		order = pmd_order;

	/* 大小按 PMD 对齐，保证 folio 完全落在映射内 */
	if (total % pmd_size)
		total = (total / pmd_size) * pmd_size;
	if (total < pmd_size)
		total = pmd_size;

	printf("=== 本次测试 ===\n");
	printf("模式              : %s\n", mode);
	printf("目标 order        : %d (%zu B)\n", order, page_size << order);
	printf("映射大小          : %zu B\n\n", total);

	show_per_size(order);

	if (do_setup) {
		printf("\n--- 配置 sysfs ---\n");
		if (setup_sysfs(order, policy))
			fprintf(stderr, "警告: sysfs 配置未完成，结果可能仍是 4K 页\n");
		show_per_size(order);
	}

	/* 1. 建 memfd 匿名文件 */
	int fd = memfd_create("largefolio_test", MFD_CLOEXEC);
	if (fd < 0) {
		perror("memfd_create");
		return 1;
	}
	if (ftruncate(fd, total)) {
		perror("ftruncate");
		return 1;
	}
	printf("\nmemfd 已创建，大小 %zu (%.1f MB)\n", total, total / 1048576.0);

	/* 2. PMD 对齐映射 */
	char *p = map_aligned(fd, total);
	if (p == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
	printf("映射地址          : %p (PMD 对齐: %s)\n", p,
	       ((uintptr_t)p % pmd_size) ? "否" : "是");

	/* 3. 打上 MADV_HUGEPAGE：advise/within_size 策略需要它，
	      always 策略下也无害 */
	if (madvise(p, total, MADV_HUGEPAGE))
		fprintf(stderr, "madvise(MADV_HUGEPAGE) 失败: %s\n", strerror(errno));

	/* 4. 触发分配 */
	unsigned long shmem_huge_before = meminfo_field("ShmemHugePages");
	unsigned long shmem_pmd_before  = meminfo_field("ShmemPmdMapped");

	if (!strcmp(mode, "write")) {
		populate_by_write(fd, total);
		/* write 之后需要再 mmap 一次才能观察到映射侧的情况；
		   这里重新映射，保证页表已建立 */
		populate_by_fault(p, total);
	} else if (!strcmp(mode, "collapse")) {
		populate_by_fault(p, total);
		if (madvise(p, total, MADV_COLLAPSE)) {
			fprintf(stderr, "madvise(MADV_COLLAPSE) 失败: %s\n", strerror(errno));
			fprintf(stderr, "  (EAGAIN/EINVAL 常见于: 内核 < 6.1、范围未对齐、"
					"或该类型不支持 collapse)\n");
		} else {
			printf("MADV_COLLAPSE 成功\n");
		}
	} else {
		populate_by_fault(p, total);
	}

	unsigned long shmem_huge_after = meminfo_field("ShmemHugePages");
	unsigned long shmem_pmd_after  = meminfo_field("ShmemPmdMapped");
	printf("meminfo ShmemHugePages : %lu -> %lu (+%lu)\n",
	       shmem_huge_before, shmem_huge_after, shmem_huge_after - shmem_huge_before);
	printf("meminfo ShmemPmdMapped : %lu -> %lu (+%lu)\n\n",
	       shmem_pmd_before, shmem_pmd_after, shmem_pmd_after - shmem_pmd_before);

	show_smaps((uintptr_t)p, mode);

	/* 5. 硬证据：统计 folio order */
	verify_open();
	if (!kf_usable) {
		printf("=== 深度验证不可用 ===\n");
		printf("无法读取 /proc/self/pagemap 或 /proc/kpageflags（通常需要 root）。\n");
		printf("请以 root 运行，或换用外部工具观察：\n");
		printf("  sudo page-types -p %d -l -N | head -40\n", getpid());
		if (!strcmp(mode, "collapse") || order == pmd_order) {
			printf("注意：上面 ShmemPmdMapped/ShmemHugePages 的增量只覆盖 PMD 阶，\n");
			printf("      mTHP（16K~512K）必须靠 kpageflags 才能确认。\n");
		}
	} else {
		int ok = verify_folios((uintptr_t)p, total, mode);
		printf("=== 结论 ===\n");
		if (ok)
			printf("✓ 检测到 large folio：memfd 映射确实拿到了大块物理连续内存\n");
		else
			printf("✗ 全是 order-0 4K 页。检查：顶层 shmem_enabled 是否 never/deny、\n"
			       "  hugepages-<N>kB/shmem_enabled 是否 never、地址是否 PMD 对齐、\n"
			       "  CONFIG_TRANSPARENT_HUGEPAGE 是否开启、内存是否已碎片化\n");
	}

	if (park) {
		printf("\n暂停中，PID=%d。另开终端执行：\n", getpid());
		printf("  sudo page-types -p %d -l -N | head -40\n", getpid());
		printf("  grep -E 'Shmem|File' /proc/meminfo\n");
		printf("按 Ctrl-C 退出\n");
		pause();
	}

	munmap(p, total);
	close(fd);
	return 0;
}
