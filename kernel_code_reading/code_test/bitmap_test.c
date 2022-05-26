#include <stdio.h>
#include "types.h"
#include "bitmap.h"

DECLARE_BITMAP(bitmaps, 35);

int main()
{
	bitmap_zero(bitmaps, 35);
	return 0;
}
