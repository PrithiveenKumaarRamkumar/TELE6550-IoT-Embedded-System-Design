#include <stdio.h>
#include <stdint.h>

/*
 * Multiply two 16-bit integers without using the * operator.
 *
 * Shift-and-add method (long multiplication in binary): look at the bits
 * of integer2 one at a time, starting from the lowest. Whenever a bit is
 * 1, add the current value of integer1 to the result. After each bit,
 * double integer1 (shift left by 1) so it lines up with the next bit.
 *
 * The loop runs at most 16 times (one per bit of integer2) and stops early
 * once no 1 bits are left. The result is stored in 32 bits because
 * 65535 x 65535 needs 32 bits.
 */
uint32_t multiply(uint16_t integer1, uint16_t integer2)
{
    uint32_t result = 0;
    uint32_t shifted = integer1;   /* 32 bits so the shifts don't overflow */

    while (integer2 != 0) {
        if (integer2 & 1) {        /* lowest bit of integer2 is 1 */
            result += shifted;
        }
        shifted <<= 1;             /* integer1 x 2, x 4, x 8, ... */
        integer2 >>= 1;            /* move on to the next bit */
    }

    return result;
}

void app_main(void)
{
    for (uint16_t a = 0; a <= 10; a++) {
        for (uint16_t b = 0; b <= 10; b++) {
            printf("%2u x %2u = %3lu\n", a, b, (unsigned long)multiply(a, b));
        }
    }
}
