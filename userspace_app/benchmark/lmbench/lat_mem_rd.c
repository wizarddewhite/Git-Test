#include	<assert.h>
#include        <ctype.h>
#include        <stdio.h>
#include        <unistd.h>
#include        <stdlib.h>
#include        <fcntl.h>
#include        <signal.h>
#include        <errno.h>
#include        <strings.h>
#include        <sys/types.h>
#include        <sys/mman.h>
#include        <sys/stat.h>
#include        <sys/wait.h>
#include	<time.h>
#include        <sys/time.h>
#include        <sys/socket.h>
#include        <sys/un.h>
#include        <sys/resource.h>
#define PORTMAP
#include	<rpc/rpc.h>
#include	<rpc/types.h>
#include 	<stdarg.h>
#ifndef HAVE_uint
typedef unsigned int uint;
#endif
typedef unsigned long long uint64;
static volatile uint64	use_result_dummy;
void
use_pointer(void *result) { use_result_dummy += (long)result; }

#define MAX_MEM_PARALLELISM 16
struct mem_state {
	char*	addr;	/* raw pointer returned by malloc */
	char*	base;	/* page-aligned pointer */
	char*	p[MAX_MEM_PARALLELISM];
	int	initialized;
	int	width;
	size_t	len;
	size_t	maxlen;
	size_t	line;
	size_t	pagesize;
	size_t	nlines;
	size_t	npages;
	size_t	nwords;
	size_t*	pages;
	size_t*	lines;
	size_t*	words;
};

void
mem_reset()
{
	//mem_benchmark_rerun = 0;
}

void
mem_cleanup(void* cookie)
{
	struct mem_state* state = (struct mem_state*)cookie;

	if (state->addr) {
		free(state->addr);
		state->addr = NULL;
	}
	if (state->lines) {
		free(state->lines);
		state->lines = NULL;
	}
	if (state->pages) {
		free(state->pages);
		state->pages = NULL;
	}
	if (state->words) {
		free(state->words);
		state->words = NULL;
	}
}

size_t*
permutation(int max, int scale)
{
	size_t	i, v;
	static size_t r = 0;
	size_t*	result = (size_t*)malloc(max * sizeof(size_t));

	if (result == NULL) return NULL;

	for (i = 0; i < max; ++i) {
		result[i] = i * (size_t)scale;
	}

	if (r == 0)
		r = (getpid()<<6) ^ getppid() ^ rand() ^ (rand()<<10);

	/* randomize the sequence */
	for (i = max - 1; i > 0; --i) {
		r = (r << 1) ^ rand();
		v = result[r % (i + 1)];
		result[r % (i + 1)] = result[i];
		result[i] = v;
	}

#ifdef _DEBUG
	fprintf(stderr, "permutation(%d): {", max);
	for (i = 0; i < max; ++i) {
	  fprintf(stderr, "%d", result[i]);
	  if (i < max - 1) 
	    fprintf(stderr, ",");
	}
	fprintf(stderr, "}\n");
	fflush(stderr);
#endif /* _DEBUG */

	return (result);
}

void
base_initialize(void* cookie)
{
	int	nwords, nlines, nbytes, npages, nmpages;
	size_t *pages;
	size_t *lines;
	size_t *words;
	struct mem_state* state = (struct mem_state*)cookie;
	register char *p = 0 /* lint */;

	state->initialized = 0;

	nbytes = state->len;
	nwords = state->line / sizeof(char*);
	nlines = state->pagesize / state->line;
	npages = (nbytes + state->pagesize - 1) / state->pagesize;
	nmpages= (state->maxlen + state->pagesize - 1) / state->pagesize;

	srand(getpid());

	words = NULL;
	lines = NULL;
	pages = permutation(nmpages, state->pagesize);
	p = state->addr = (char*)malloc(state->maxlen + 2 * state->pagesize);

	state->nwords = nwords;
	state->nlines = nlines;
	state->npages = npages;
	state->lines = lines;
	state->pages = pages;
	state->words = words;

	if (state->addr == NULL || pages == NULL)
		return;

	if ((unsigned long)p % state->pagesize) {
		p += state->pagesize - (unsigned long)p % state->pagesize;
	}
	state->base = p;
	state->initialized = 1;
	mem_reset();
}

void
stride_initialize(void* cookie)
{
	struct mem_state* state = (struct mem_state*)cookie;
	size_t	i;
	size_t	range = state->len;
	size_t	stride = state->line;
	char*	addr;

	base_initialize(cookie);
	if (!state->initialized) return;
	addr = state->base;

	for (i = stride; i < range; i += stride) {
		*(char **)&addr[i - stride] = (char*)&addr[i];
	}
	*(char **)&addr[i - stride] = (char*)&addr[0];
	state->p[0] = addr;
	mem_reset();
}

#define	ONE	p = (char **)*p;
#define	FIVE	ONE ONE ONE ONE ONE
#define	TEN	FIVE FIVE
#define	FIFTY	TEN TEN TEN TEN TEN
#define	HUNDRED	FIFTY FIFTY

void
benchmark_loads(void *cookie)
{
	struct mem_state* state = (struct mem_state*)cookie;
	register char **p = (char**)state->p[0];
	register size_t i;
	register size_t count = state->len / (state->line * 100) + 1;
	//printf("benchmark will excute %d loop\n", count);

	for (i = 0; i < count; ++i) {
		HUNDRED;
	}

	//use_pointer((void *)p);
	state->p[0] = (char*)p;
}

static struct timeval 	start_tv, stop_tv;

/*
 * Make the time spend be usecs.
 */
void
settime(uint64 usecs)
{
	bzero((void*)&start_tv, sizeof(start_tv));
	stop_tv.tv_sec = usecs / 1000000;
	stop_tv.tv_usec = usecs % 1000000;
}

void
tvsub(struct timeval * tdiff, struct timeval * t1, struct timeval * t0)
{
	tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
	tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
	if (tdiff->tv_usec < 0 && tdiff->tv_sec > 0) {
		tdiff->tv_sec--;
		tdiff->tv_usec += 1000000;
		assert(tdiff->tv_usec >= 0);
	}

	/* time shouldn't go backwards!!! */
	if (tdiff->tv_usec < 0 || t1->tv_sec < t0->tv_sec) {
		tdiff->tv_sec = 0;
		tdiff->tv_usec = 0;
	}
}

uint64
tvdelta(struct timeval *start, struct timeval *stop)
{
	struct timeval td;
	uint64	usecs;

	tvsub(&td, stop, start);
	usecs = td.tv_sec;
	usecs *= 1000000;
	usecs += td.tv_usec;
	return (usecs);
}

/*
 * Start timing now.
 */
void
start(struct timeval *tv)
{
	if (tv == NULL) {
		tv = &start_tv;
	}
#ifdef	RUSAGE
	getrusage(RUSAGE_SELF, &ru_start);
#endif
	(void) gettimeofday(tv, (struct timezone *) 0);
}

/*
 * Stop timing and return real time in microseconds.
 */
uint64
stop(struct timeval *begin, struct timeval *end)
{
	if (end == NULL) {
		end = &stop_tv;
	}
	(void) gettimeofday(end, (struct timezone *) 0);
#ifdef	RUSAGE
	getrusage(RUSAGE_SELF, &ru_stop);
#endif

	if (begin == NULL) {
		begin = &start_tv;
	}
	return (tvdelta(begin, end));
}

size_t
step(size_t k)
{
	if (k < 1024) {
		k = k * 2;
        } else if (k < 4*1024) {
		k += 1024;
	} else {
		size_t s;

		for (s = 32 * 1024; s <= k; s *= 2)
			;
		k += s / 16;
	}
	return (k);
}

void loads(size_t len, size_t range, size_t stride)
{
	size_t i;
	size_t count;
	struct mem_state state;

	if (range < stride)
		return;

	state.width = 1;
	state.len = range;
	state.maxlen = len;
	state.line = stride;
	state.pagesize = getpagesize();
	count = 100 * (state.len / (state.line * 100) + 1);

	stride_initialize(&state);

	settime(0);
	start(NULL);
	benchmark_loads(&state);
	printf("range: %.5f, pased %.3f\n",
		range / (1024. * 1024.), 
		(1000. * stop(NULL, NULL)));
	mem_cleanup(&state);
}

char
last(char *s)
{
	while (*s++)
		;
	return (s[-2]);
}

uint64
bytes(char *s)
{
	uint64	n;

	if (sscanf(s, "%llu", &n) < 1)
		return (0);

	if ((last(s) == 'k') || (last(s) == 'K'))
		n *= 1024;
	if ((last(s) == 'm') || (last(s) == 'M'))
		n *= (1024 * 1024);
	return (n);
}

int main(int ac, char **av)
{
	size_t stride, max_len, range;

	if (ac != 3)
		exit(-1);

        max_len = atoi(av[1]);
	max_len *= 1024 * 1024;
	stride = bytes(av[2]);
	printf("max_len %d, stride %d\n", max_len, stride);

	for (range = 512; range <= max_len; range = step(range)) {
		loads(max_len, range, stride);
	}

	return 0;
}
