#include <stdio.h>

int func_0(void)
{
    printf("%s: %d\n", __FUNCTION__, __LINE__);
    return 0;
}

int func_1(void)
{
    printf("%s: %d\n", __FUNCTION__, __LINE__);
    return 0;
}

int func_2(void)
{
    printf("%s: %d\n", __FUNCTION__, __LINE__);
    return 0;
}

int func_3(void)
{
    printf("%s: %d\n", __FUNCTION__, __LINE__);
    return 0;
}

void main(void)
{
        func_0();
        func_3();
}
