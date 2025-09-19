#include "adc_linmap.h"
#include <stdint.h>
#include <stdio.h>

#define MILLI_VOLT_REFERENCE (3300L)
#define ADC_BIT_COUNT 10
#define SCALING_FACTOR 10

// Test a mapping with a given type width
#define RUN_TEST(type, label)                                                                                                                                                                          \
    do                                                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
        printf("## Fixed Linear Conversion (" label ")\n\n");                                                                                                                                          \
                                                                                                                                                                                                       \
        printf("### ADC Value --> Millivolt --> ADC Value \n\n");                                                                                                                                      \
        printf("| ADC Value | Millivolts (Fixed Point) | ADC From mV (Fixed Point) | error |\n");                                                                                                      \
        printf("|-----------|--------------------------|---------------------------|-------|\n");                                                                                                      \
        for (unsigned adc_val = 0; adc_val <= (1 << ADC_BIT_COUNT); adc_val += 64)                                                                                                                     \
        {                                                                                                                                                                                              \
            type adc_typed = (type)adc_val;                                                                                                                                                            \
            type millivolt_fixed = ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, adc_typed);                                                                 \
            type adc_from_mv_fixed = ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, millivolt_fixed);                                                         \
            printf("| %8u | %24u | %25u | %5d |\n", (unsigned)adc_val, (unsigned)millivolt_fixed, (unsigned)adc_from_mv_fixed, (int)adc_from_mv_fixed - (int)adc_typed);                               \
        }                                                                                                                                                                                              \
        printf("\n\n");                                                                                                                                                                                \
                                                                                                                                                                                                       \
        printf("### Millivolt --> ADC Value --> Millivolt \n\n");                                                                                                                                      \
        printf("| MV Value     | ADC From mV (Fixed Point) | Millivolts (Fixed Point) | error |\n");                                                                                                   \
        printf("|--------------|---------------------------|--------------------------|-------|\n");                                                                                                   \
        for (unsigned mv_val = 0; mv_val <= MILLI_VOLT_REFERENCE; mv_val += 100)                                                                                                                       \
        {                                                                                                                                                                                              \
            type mv_typed = (type)mv_val;                                                                                                                                                              \
            type adc_from_mv_fixed = ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, mv_typed);                                                                \
            type millivolt_fixed = ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, adc_from_mv_fixed);                                                         \
            printf("| %8u mV | %25u | %24u | %5d |\n", (unsigned)mv_val, (unsigned)adc_from_mv_fixed, (unsigned)millivolt_fixed, (int)millivolt_fixed - (int)mv_typed);                                \
        }                                                                                                                                                                                              \
        printf("\n\n");                                                                                                                                                                                \
    } while (0)

int main(void)
{
    printf("# ADC CONVERSION MACRO OUTPUT EXAMPLE\n\n");
    printf(" - MILLI_VOLT_REFERENCE : %ld\n", MILLI_VOLT_REFERENCE);
    printf(" - ADC_BIT_COUNT        : %d\n", ADC_BIT_COUNT);
    printf(" - SCALING_FACTOR       : %d\n", SCALING_FACTOR);
    printf("\n");

    // Run with different simulated integer widths
    RUN_TEST(uint8_t, "8-bit");
    RUN_TEST(uint16_t, "16-bit");
    RUN_TEST(uint32_t, "32-bit");

    printf("## Floating Linear Conversion (reference)\n\n");
    printf("### ADC Value --> Millivolt --> ADC Value \n\n");
    printf("| ADC Value | Millivolts (float) | ADC From mV (int) | error |\n");
    printf("|-----------|--------------------|-------------------|-------|\n");
    for (uint16_t adc_val = 0; adc_val <= (1 << ADC_BIT_COUNT); adc_val += 64)
    {
        float millivolt_float = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, (float)MILLI_VOLT_REFERENCE, (float)adc_val);
        uint16_t adc_from_mv_int = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, millivolt_float);
        printf("| %8u | %18.2f mV | %17u | %5d |\n", adc_val, millivolt_float, adc_from_mv_int, adc_from_mv_int - adc_val);
    }
    printf("\n\n");

    printf("### Millivolt --> ADC Value --> Millivolt \n\n");
    printf("| MV Value | ADC From mV (int) | Millivolts (float) | error |\n");
    printf("|----------|-------------------|--------------------|-------|\n");
    for (uint16_t mv_val = 0; mv_val <= MILLI_VOLT_REFERENCE; mv_val += 100)
    {
        uint16_t adc_from_mv_int = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, mv_val);
        float millivolt_float = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, (float)MILLI_VOLT_REFERENCE, (float)adc_from_mv_int);
        printf("| %6u mV | %17u | %18.2f mV | %5.2f |\n", mv_val, adc_from_mv_int, millivolt_float, millivolt_float - (float)mv_val);
    }
    printf("\n\n");

    return 0;
}
