// mul.c
#include <stdint.h>

int64_t mul(int32_t value1, int32_t value2)
{
    /* widen first so the product is done in 64 bits and can't overflow */
    return (int64_t)value1 * value2;
}
