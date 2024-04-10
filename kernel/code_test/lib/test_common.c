#include <string.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include "test_common.h"

#define PREFIXES_MAX				15
#define DELIM					": "

static const char /* __maybe_unused */ *prefixes[PREFIXES_MAX];
static int /* __maybe_unused */ nr_prefixes;

static const char *short_opts = "hv";
static const struct option long_opts[] = {
	{"help", 0, NULL, 'h'},
	{"verbose", 0, NULL, 'v'},
	{NULL, 0, NULL, 0}
};

static const char * const help_opts[] = {
	"display this help message and exit",
	"disallow allocations from regions marked as hotplugged\n\t\t\t"
		"by simulating enabling the \"movable_node\" kernel\n\t\t\t"
		"parameter",
	"enable verbose output, which includes the name of the\n\t\t\t"
		"memblock function being tested, the name of the test,\n\t\t\t"
		"and whether the test passed or failed."
};

static int verbose;

static void usage(const char *prog)
{
	// BUILD_BUG_ON(ARRAY_SIZE(help_opts) != ARRAY_SIZE(long_opts) - 1);

	printf("Usage: %s [-%s]\n", prog, short_opts);

	for (int i = 0; long_opts[i].name; i++) {
		printf("  -%c, --%-12s\t%s\n", long_opts[i].val,
		       long_opts[i].name, help_opts[i]);
	}

	exit(1);
}

void parse_args(int argc, char **argv)
{
	int c;

	while ((c = getopt_long_only(argc, argv, short_opts, long_opts,
				     NULL)) != -1) {
		switch (c) {
		case 'v':
			verbose = 1;
			break;
		default:
			usage(argv[0]);
		}
	}
}

void print_prefixes(const char *postfix)
{
	for (int i = 0; i < nr_prefixes; i++)
		test_print("%s%s", prefixes[i], DELIM);
	test_print(postfix);
}

void test_fail(void)
{
	if (verbose) {
		// ksft_test_result_fail(": ");
		printf("fail : ");
		print_prefixes("failed\n");
	}
}

void test_pass(void)
{
	if (verbose) {
		// ksft_test_result_pass(": ");
		printf("ok : ");
		print_prefixes("passed\n");
	}
}

void test_print(const char *fmt, ...)
{
	if (verbose) {
		int saved_errno = errno;
		va_list args;

		va_start(args, fmt);
		errno = saved_errno;
		vprintf(fmt, args);
		va_end(args);
	}
}

void prefix_reset(void)
{
	memset(prefixes, 0, PREFIXES_MAX * sizeof(char *));
	nr_prefixes = 0;
}

void prefix_push(const char *prefix)
{
	assert(nr_prefixes < PREFIXES_MAX);
	prefixes[nr_prefixes] = prefix;
	nr_prefixes++;
}

void prefix_pop(void)
{
	if (nr_prefixes > 0) {
		prefixes[nr_prefixes - 1] = 0;
		nr_prefixes--;
	}
}
