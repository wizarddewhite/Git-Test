#ifndef _TEST_COMMON_H
#define _TEST_COMMON_H

#include <assert.h>

/**
 * ASSERT_EQ():
 * Check the condition
 * @_expected == @_seen
 * If false, print failed test message (if running with --verbose) and then
 * assert.
 */
#define ASSERT_EQ(_expected, _seen) do { \
	if ((_expected) != (_seen)) \
		test_fail(); \
	assert((_expected) == (_seen)); \
} while (0)

#define ASSERT_TRUE(_seen) ASSERT_EQ(true, _seen)
#define ASSERT_FALSE(_seen) ASSERT_EQ(false, _seen)

/**
 * ASSERT_NE():
 * Check the condition
 * @_expected != @_seen
 * If false, print failed test message (if running with --verbose) and then
 * assert.
 */
#define ASSERT_NE(_expected, _seen) do { \
	if ((_expected) == (_seen)) \
		test_fail(); \
	assert((_expected) != (_seen)); \
} while (0)

/**
 * ASSERT_LT():
 * Check the condition
 * @_expected < @_seen
 * If false, print failed test message (if running with --verbose) and then
 * assert.
 */
#define ASSERT_LT(_expected, _seen) do { \
	if ((_expected) >= (_seen)) \
		test_fail(); \
	assert((_expected) < (_seen)); \
} while (0)

/**
 * ASSERT_LE():
 * Check the condition
 * @_expected <= @_seen
 * If false, print failed test message (if running with --verbose) and then
 * assert.
 */
#define ASSERT_LE(_expected, _seen) do { \
	if ((_expected) > (_seen)) \
		test_fail(); \
	assert((_expected) <= (_seen)); \
} while (0)

/**
 * ASSERT_MEM_EQ():
 * Check that the first @_size bytes of @_seen are all equal to @_expected.
 * If false, print failed test message (if running with --verbose) and then
 * assert.
 */
#define ASSERT_MEM_EQ(_seen, _expected, _size) do { \
	for (int _i = 0; _i < (_size); _i++) { \
		ASSERT_EQ(((char *)_seen)[_i], (_expected)); \
	} \
} while (0)

/**
 * ASSERT_MEM_NE():
 * Check that none of the first @_size bytes of @_seen are equal to @_expected.
 * If false, print failed test message (if running with --verbose) and then
 * assert.
 */
#define ASSERT_MEM_NE(_seen, _expected, _size) do { \
	for (int _i = 0; _i < (_size); _i++) { \
		ASSERT_NE(((char *)_seen)[_i], (_expected)); \
	} \
} while (0)

#define PREFIX_PUSH() prefix_push(__func__)

void parse_args(int argc, char **argv);

void test_fail(void);
void test_pass(void);
void test_print(const char *fmt, ...);
void prefix_reset(void);
void prefix_push(const char *prefix);
void prefix_pop(void);

static inline void test_pass_pop(void)
{
	test_pass();
	prefix_pop();
}

#endif // _TEST_COMMON_H
