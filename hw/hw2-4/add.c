// add.c
#include <stdint.h>

int64_t add(int32_t value1, int32_t value2)
{
    /* widen first so the sum is done in 64 bits and can't overflow */
    return (int64_t)value1 + value2;
}
