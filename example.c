#include "adc_linmap.h"
#include <stdint.h>
#include <stdio.h>

#define MILLI_VOLT_REFERENCE (3300L)
#define ADC_BIT_COUNT        10
#define SCALING_FACTOR        8  /* max safe for 10-bit ADC + 3300 mV with int32_t */

int main(void)
{
    printf("# ADC CONVERSION MACRO OUTPUT EXAMPLE\n\n");
    printf(" - MILLI_VOLT_REFERENCE : %ld\n", MILLI_VOLT_REFERENCE);
    printf(" - ADC_BIT_COUNT        : %d\n", ADC_BIT_COUNT);
    printf("\n");

    /* --- Fast macros (recommended): multiply + shift, no division --------- */
    printf("## Fast Linear Conversion (int32_t, multiply+shift)\n\n");
    printf("Error vs exact: <0.13%% -- well within ADC noise\n\n");

    printf("### ADC Value --> Millivolt\n\n");
    printf("| ADC Value | Millivolts (fast) | Millivolts (float ref) | error |\n");
    printf("|-----------|-------------------|------------------------|-------|\n");
    for (unsigned adc_val = 0; adc_val <= (1u << ADC_BIT_COUNT); adc_val += 64)
    {
        int32_t  mv_fast  = ADC_MILLIVOLT_FROM_VAL_FAST(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, (int32_t)adc_val);
        float    mv_float = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, (float)MILLI_VOLT_REFERENCE, (float)adc_val);
        printf("| %9u | %17ld | %22.2f | %5ld |\n",
               adc_val, (long)mv_fast, mv_float, (long)mv_fast - (long)mv_float);
    }
    printf("\n\n");

    printf("### Millivolt --> ADC Value\n\n");
    printf("| MV Value  | ADC (fast) | ADC (float ref) | error |\n");
    printf("|-----------|------------|-----------------|-------|\n");
    for (unsigned mv_val = 0; mv_val <= (unsigned)MILLI_VOLT_REFERENCE; mv_val += 100)
    {
        int32_t  adc_fast  = ADC_VAL_FROM_MILLIVOLT_FAST(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, (int32_t)mv_val);
        uint16_t adc_float = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, (float)mv_val);
        printf("| %7u mV | %10ld | %15u | %5ld |\n",
               mv_val, (long)adc_fast, adc_float, (long)adc_fast - adc_float);
    }
    printf("\n\n");

    /* --- Exact macros (legacy): divide by (1<<N)-1, slower on AVR --------- */
    printf("## Exact Fixed-Point Conversion (int32_t, SCALE=%d)\n\n", SCALING_FACTOR);

    printf("### ADC Value --> Millivolt --> ADC Value \n\n");
    printf("| ADC Value | Millivolts (fixed) | ADC back (fixed) | error |\n");
    printf("|-----------|--------------------|------------------|-------|\n");
    for (unsigned adc_val = 0; adc_val <= (1u << ADC_BIT_COUNT); adc_val += 64)
    {
        int32_t mv   = ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, (int32_t)adc_val);
        int32_t back = ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, mv);
        printf("| %9u | %18ld | %16ld | %5ld |\n",
               adc_val, (long)mv, (long)back, (long)back - (long)adc_val);
    }
    printf("\n\n");

    /* --- Float reference --------------------------------------------------- */
    printf("## Floating-Point Reference\n\n");

    printf("### ADC Value --> Millivolt --> ADC Value \n\n");
    printf("| ADC Value | Millivolts (float) | ADC From mV (int) | error |\n");
    printf("|-----------|--------------------|-------------------|-------|\n");
    for (uint16_t adc_val = 0; adc_val <= (1u << ADC_BIT_COUNT); adc_val += 64)
    {
        float    mv      = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, (float)MILLI_VOLT_REFERENCE, (float)adc_val);
        uint16_t adc_bk  = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, mv);
        printf("| %9u | %18.2f mV | %17u | %5d |\n", adc_val, mv, adc_bk, (int)adc_bk - adc_val);
    }
    printf("\n\n");

    printf("### Millivolt --> ADC Value --> Millivolt \n\n");
    printf("| MV Value  | ADC From mV (int) | Millivolts (float) | error |\n");
    printf("|-----------|-------------------|--------------------|-------|\n");
    for (uint16_t mv_val = 0; mv_val <= (uint16_t)MILLI_VOLT_REFERENCE; mv_val += 100)
    {
        uint16_t adc = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, mv_val);
        float    mv  = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, (float)MILLI_VOLT_REFERENCE, (float)adc);
        printf("| %7u mV | %17u | %18.2f mV | %5.2f |\n", mv_val, adc, mv, mv - (float)mv_val);
    }
    printf("\n\n");

    return 0;
}
