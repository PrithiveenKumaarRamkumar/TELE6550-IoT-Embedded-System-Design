#include <stdio.h>
#include <stdint.h>

/*
 * Add two 16-bit integers without using the + or - operators.
 *
 * Works like an adder circuit in hardware:
 *   - XOR (^) adds the bits without carrying:   0+0=0, 0+1=1, 1+0=1, 1+1=0
 *   - AND (&) finds where both bits are 1, which is where a carry happens.
 *     Shifting the carries left by 1 moves each one into the next column.
 * Repeat with the partial sum and the carries until there are no carries
 * left. The loop runs at most 17 times.
 *
 * 32-bit variables are used because 65535 + 65535 = 131070 needs 17 bits.
 */
uint32_t add(uint16_t integer1, uint16_t integer2)
{
    uint32_t sum = integer1;
    uint32_t carry = integer2;

    while (carry != 0) {
        uint32_t carries = (sum & carry) << 1;   /* columns that carry a 1 */
        sum = sum ^ carry;                       /* add without carrying   */
        carry = carries;                         /* add the carries next   */
    }

    return sum;
}

void app_main(void)
{
    for (uint16_t a = 0; a <= 10; a++) {
        for (uint16_t b = 0; b <= 10; b++) {
            printf("%2u + %2u = %2lu\n", a, b, (unsigned long)add(a, b));
        }
    }
}
