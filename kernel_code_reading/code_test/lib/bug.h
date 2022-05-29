#ifndef _ASM_GENERIC_BUG_H
#define _ASM_GENERIC_BUG_H
#include <stdlib.h>
#include "kernel.h"

#define TAINT_WARN			9
#define CUT_HERE		"------------[ cut here ]------------\n"

void panic(const char *fmt, ...);

void warn_slowpath_fmt(const char *file, const int line, unsigned taint,
		       const char *fmt, ...);
#define __WARN()		__WARN_printf(TAINT_WARN, NULL)
#define __WARN_printf(taint, arg...) do {				\
		warn_slowpath_fmt(__FILE__, __LINE__, taint, arg);	\
	} while (0)

#ifndef WARN_ON
#define WARN_ON(condition) ({						\
	int __ret_warn_on = !!(condition);				\
	if (unlikely(__ret_warn_on))					\
		__WARN();						\
	unlikely(__ret_warn_on);					\
})
#endif

#define WARN_ON_ONCE(condition) WARN_ON(condition)

#ifndef WARN
#define WARN(condition, format...) ({					\
	int __ret_warn_on = !!(condition);				\
	if (unlikely(__ret_warn_on))					\
		__WARN_printf(TAINT_WARN, format);			\
	unlikely(__ret_warn_on);					\
})
#endif

#define BUG() do { \
	printf("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
	abort(); \
} while (0)

#define BUG_ON(condition) do { if (condition) BUG(); } while (0)

#endif // _ASM_GENERIC_BUG_H
