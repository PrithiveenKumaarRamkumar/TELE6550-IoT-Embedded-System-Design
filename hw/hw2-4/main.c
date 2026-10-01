// main.c
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

// Declarations of add and mul (defined in add.c and mul.c)
int64_t add(int32_t value1, int32_t value2);
int64_t mul(int32_t value1, int32_t value2);

void app_main(void)
{
    // 10 x (14 + 10) + 15 x (4 + 8)
    int64_t result = add(mul(10, add(14, 10)), mul(15, add(4, 8)));

    printf("10 x (14 + 10) + 15 x (4 + 8) = %" PRId64 "\n", result);
}
