#include <stdint.h>
#include <stdio.h>

#include "adc_linmap.h"

#define MILLI_VOLT_REFERENCE (3300L)
#define ADC_BIT_COUNT 10
#define SCALING_FACTOR 10

int main()
{
    printf("# ADC CONVERSION MACRO OUTPUT EXAMPLE\n\n");

    printf(" - MILLI_VOLT_REFERENCE : %ld\n", MILLI_VOLT_REFERENCE);
    printf(" - ADC_BIT_COUNT        : %d\n", ADC_BIT_COUNT);
    printf(" - SCALING_FACTOR       : %d\n", SCALING_FACTOR);
    printf("\n");

    printf("## Fixed Linear Conversion\n\n");

    printf("### ADC Value --> Millivolt --> ADC Value \n\n");
    printf("| ADC Value | Millivolts (Fixed Point) | ADC From mV (Fixed Point) | error |\n");
    printf("|-----------|--------------------------|---------------------------|-------|\n");
    for (uint16_t adc_val = 0; adc_val <= (1 << ADC_BIT_COUNT); adc_val += 64)
    {
        uint16_t millivolt_fixed = ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, adc_val);
        uint16_t adc_from_mv_fixed = ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, millivolt_fixed);
        printf("|  %8u |   %22u | %25d | %5d |\n", adc_val, millivolt_fixed, adc_from_mv_fixed, adc_from_mv_fixed - adc_val);
    }
    printf("\n\n");

    printf("### Millivolt --> ADC Value --> Millivolt \n\n");
    printf("| MV Value     | ADC From mV (Fixed Point) | Millivolts (Fixed Point) | error |\n");
    printf("|--------------|---------------------------|--------------------------|-------|\n");
    for (uint16_t mv_val = 0; mv_val <= MILLI_VOLT_REFERENCE; mv_val += 100)
    {
        uint16_t adc_from_mv_fixed = ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, mv_val);
        uint16_t millivolt_fixed = ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, adc_from_mv_fixed);
        printf("|  %8u mV | %25d |   %22u | %5d |\n", mv_val, adc_from_mv_fixed, millivolt_fixed, (int)millivolt_fixed - (int)mv_val);
    }
    printf("\n\n");

#if 0 // No issue?
    printf("## Integer Linear Conversion\n\n");
    printf("Note: Interesting to see less accuracy here... but this is why we use floats or fixed point\n\n");

    printf("### ADC Value --> Millivolt --> ADC Value \n\n");
    printf("| ADC Value | Millivolts (Integer)   | ADC From mV (Integer)  | error |\n");
    printf("|-----------|------------------------|------------------------|-------|\n");
    for (uint16_t adc_val = 0; adc_val <= (1 << ADC_BIT_COUNT); adc_val += 64)
    {
        uint16_t millivolt_int = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, adc_val);
        uint16_t adc_from_mv_int = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, millivolt_int);
        printf("|  %8u | %19u mV | %22d | %5d |\n", adc_val, millivolt_int, adc_from_mv_int, adc_from_mv_int - adc_val);
    }
    printf("\n\n");

    printf("### Millivolt --> ADC Value --> Millivolt \n\n");
    printf("| MV Value     | ADC From mV (Integer)  | Millivolts (Integer)   | error |\n");
    printf("|--------------|------------------------|------------------------|-------|\n");
    for (uint16_t mv_val = 0; mv_val <= MILLI_VOLT_REFERENCE; mv_val += 100)
    {
        uint16_t adc_from_mv_int = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, mv_val);
        uint16_t millivolt_int = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, adc_from_mv_int);
        printf("|  %8u mV | %22d | %19u mV | %5d |\n", mv_val, adc_from_mv_int, millivolt_int, (int)millivolt_int - (int)mv_val);
    }
    printf("\n\n");
#endif

    printf("## Floating Linear Conversion\n\n");

    printf("### ADC Value --> Millivolt --> ADC Value \n\n");
    printf("| ADC Value | Millivolts (dfloat)    | ADC From mV (Integer)  | error |\n");
    printf("|-----------|------------------------|------------------------|-------|\n");
    for (uint16_t adc_val = 0; adc_val <= (1 << ADC_BIT_COUNT); adc_val += 64)
    {
        float millivolt_int = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, (float)MILLI_VOLT_REFERENCE, (float)adc_val);
        uint16_t adc_from_mv_int = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, millivolt_int);
        printf("|  %8u | %19.2lf mV | %22d | %5d |\n", adc_val, millivolt_int, adc_from_mv_int, adc_from_mv_int - adc_val);
    }
    printf("\n\n");

    printf("### Millivolt --> ADC Value --> Millivolt \n\n");
    printf("| MV Value     | ADC From mV (Integer)  | Millivolts (dfloat)    | error |\n");
    printf("|--------------|------------------------|------------------------|-------|\n");
    for (uint16_t mv_val = 0; mv_val <= MILLI_VOLT_REFERENCE; mv_val += 100)
    {
        uint16_t adc_from_mv_int = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, mv_val);
        float millivolt = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, (float)MILLI_VOLT_REFERENCE, (float)adc_from_mv_int);
        printf("|  %8u mV | %22d | %19.2lf mV | %5.2f |\n", mv_val, adc_from_mv_int, millivolt, (float)millivolt - (float)mv_val);
    }
    printf("\n\n");

    return 0;
}
