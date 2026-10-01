// main.c
#include <stdint.h>
#include <stdio.h>

typedef int32_t BMP280_S32_t;
typedef int64_t BMP280_S64_t;
typedef uint32_t BMP280_U32_t;
typedef uint64_t BMP280_U64_t;

// Calibration values (normally read from the sensor's memory)
unsigned short dig_T1 = 27504;
short dig_T2 = 26435;
short dig_T3 = -1000;

// From the BMP280 datasheet (section 3.11.3 / 8.2):
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of "5123" equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t t_fine;
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
    BMP280_S32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((BMP280_S32_t)dig_T1 << 1))) * ((BMP280_S32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BMP280_S32_t)dig_T1)) * ((adc_T >> 4) - ((BMP280_S32_t)dig_T1))) >> 12) *
            ((BMP280_S32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

void app_main(void)
{
    BMP280_S32_t adc_T = 550000;   // raw temperature reading

    BMP280_S32_t T = bmp280_compensate_T_int32(adc_T);

    // T is in hundredths of a degree, e.g. 3451 means 34.51 DegC
    printf("Raw value: %ld\n", (long)adc_T);
    printf("T = %ld  ->  %ld.%02ld DegC\n", (long)T, (long)(T / 100), (long)(T % 100));
}
