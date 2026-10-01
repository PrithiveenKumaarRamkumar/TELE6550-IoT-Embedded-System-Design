// main_simple.c
#include <stdint.h>
#include <stdio.h>

void app_main(void)
{
    // Set the 3rd bit of 0x2105 (3rd bit = 0000 0000 0000 0100 = 0x0004)
    uint16_t a = 0x2105;
    a = a | 0x0004;
    printf("Set 3rd bit of 0x2105:        0x%04X\n", a);

    // Clear the 4th bit of 0x21ff (everything except the 4th bit = 0xFFF7)
    uint16_t b = 0x21ff;
    b = b & 0xFFF7;
    printf("Clear 4th bit of 0x21FF:      0x%04X\n", b);

    // Flip the 5th bit of 0x2100 (5th bit = 0000 0000 0001 0000 = 0x0010)
    uint16_t c = 0x2100;
    c = c ^ 0x0010;
    printf("Flip 5th bit of 0x2100:       0x%04X\n", c);

    // Set the 2nd and 5th bits of 0x80 (bitmask 0001 0010 = 0x12)
    uint8_t d = 0x80;
    d = d | 0x12;
    printf("Set 2nd & 5th bits of 0x80:   0x%02X\n", d);

    // Clear the 1st and 7th bits of 0xff (bitmask 0100 0001 = 0x41,
    // so AND with everything except those bits: 1011 1110 = 0xBE)
    uint8_t e = 0xff;
    e = e & 0xBE;
    printf("Clear 1st & 7th bits of 0xFF: 0x%02X\n", e);

    // Flip the 3rd and 4th bits of 0x00 (bitmask 0000 1100 = 0x0C)
    uint8_t f = 0x00;
    f = f ^ 0x0C;
    printf("Flip 3rd & 4th bits of 0x00:  0x%02X\n", f);
}
