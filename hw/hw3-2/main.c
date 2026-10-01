// main.c
#include <stdint.h>
#include <stdio.h>

// Swap the 2nd and 4th bits (1st bit = LSB), passing the word by value.
// 2nd bit = 0000 0010 = 0x02, 4th bit = 0000 1000 = 0x08
uint8_t swapBitsByValue(uint8_t word)
{
    uint8_t bit2 = (word >> 1) & 1;
    uint8_t bit4 = (word >> 3) & 1;

    // If the two bits are different, flipping both swaps them.
    // If they are the same, swapping changes nothing.
    if (bit2 != bit4) {
        word = word ^ 0x0A;   // 0000 1010 = 2nd and 4th bits
    }

    return word;
}

// Same swap, but passing the word by reference (a pointer to it).
// The function changes the caller's variable directly.
void swapBitsByReference(uint8_t *word)
{
    uint8_t bit2 = (*word >> 1) & 1;
    uint8_t bit4 = (*word >> 3) & 1;

    if (bit2 != bit4) {
        *word = *word ^ 0x0A;
    }
}

void app_main(void)
{
    int mismatches = 0;

    printf("input  by value  by reference\n");

    // i is an int, not a uint8_t: a uint8_t can never be greater than
    // 0xff, so "i <= 0xff" would always be true and the loop would never end.
    for (int i = 0x00; i <= 0xff; i++) {
        uint8_t byValue = swapBitsByValue(i);

        uint8_t byReference = i;
        swapBitsByReference(&byReference);

        printf("0x%02X   0x%02X      0x%02X\n", i, byValue, byReference);

        if (byValue != byReference) {
            mismatches++;
        }
    }

    if (mismatches == 0) {
        printf("Both functions gave identical results for all 256 values.\n");
    } else {
        printf("%d values did not match!\n", mismatches);
    }
}
