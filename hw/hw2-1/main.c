#include <stdio.h>
#include <stdint.h>

/*
 * Count the number of set bits in a 32-bit integer.
 *
 * Brian Kernighan's method: "integer & (integer - 1)" clears the lowest
 * set bit. We repeat that until the number becomes 0, counting how many
 * times we did it. The loop runs once per set bit, not once per bit, so
 * a number with few 1s finishes quickly.
 */
int countBits(uint32_t integer)
{
    int count = 0;

    while (integer != 0) {
        integer = integer & (integer - 1);   /* remove the lowest 1 bit */
        count++;
    }

    return count;
}

void app_main(void)
{
    for (uint32_t i = 0; i <= 255; i++) {
        printf("%3lu: %d\n", (unsigned long)i, countBits(i));
    }
}
