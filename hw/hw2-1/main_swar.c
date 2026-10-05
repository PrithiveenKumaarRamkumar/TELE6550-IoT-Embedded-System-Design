#include <stdio.h>
#include <stdint.h>

/*
 * Count the number of set bits in a 32-bit integer.
 *
 * SWAR ("SIMD Within A Register"): treat the 32-bit number as 32 one-bit
 * counters side by side, then add neighbouring counters in parallel,
 * doubling their width each step: 1 -> 2 -> 4 -> 8 bits. A final multiply
 * adds the four 8-bit counters together. No loops or branches, so 0 takes
 * the same time as 0xFFFFFFFF.
 *
 * Worked example, 0x12345678 (13 bits set):
 *   step 1  0x11245564   16 two-bit counts
 *   step 2  0x11212231    8 four-bit counts
 *   step 3  0x02030404    4 byte counts: 2 3 4 4
 *   step 4  (0x02030404 * 0x01010101) >> 24 = 2 + 3 + 4 + 4 = 13
 */
int countBits(uint32_t integer)
{
    integer = integer - ((integer >> 1) & 0x55555555u);                 /* 2-bit sums */
    integer = (integer & 0x33333333u) + ((integer >> 2) & 0x33333333u); /* 4-bit sums */
    integer = (integer + (integer >> 4)) & 0x0F0F0F0Fu;                 /* 8-bit sums */
    return (int)((integer * 0x01010101u) >> 24);                        /* add bytes  */
}

void app_main(void)
{
    for (uint32_t i = 0; i <= 255; i++) {
        printf("%3lu: %d\n", (unsigned long)i, countBits(i));
    }
}
