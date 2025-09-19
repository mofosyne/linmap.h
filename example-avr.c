/*
 * avr-example.c
 * Minimal single-file AVR test for LINMAP fixed-point conversions
 * Uses printf redirected to UART0
 */

#include "adc_linmap.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/sleep.h>

/* -------- LINMAP MACROS (from your library) -------- */

#define ADC_BIT_COUNT 8
#define MILLI_VOLT_REFERENCE 5000
#define SCALING_FACTOR 8
#define SAMPLE_COUNT 16

/* -------- UART Setup -------- */
void uart_init(void)
{
    UBRR0 = 103;           // 16MHz / 16 / 9600 - 1
    UCSR0B = (1 << TXEN0); // Enable transmitter
}

int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream); // CRLF
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

/* -------- Main -------- */
int main(void)
{
    stdout = &uart_output; // redirect printf to UART
    uart_init();

    printf("## Linear Conversion (AVR 8-bit e.g. atmega328p)\n\n");
    printf("| ADC | Millivolt | ADC back | error |\n");
    printf("|-----|-----------|----------|-------|\n");
    for (int i = 0; i <= (1 << ADC_BIT_COUNT); i += 64)                                                                                                                     \
    {
        uint8_t adc_val = (int)i;
        uint8_t millivolt_int = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, adc_val);
        uint8_t adc_from_mv_int = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, millivolt_int);
        int error = (int)adc_from_mv_int - (int)adc_val;
        printf("| %3u | %9u | %8u | %5d |\n", adc_val, millivolt_int, adc_from_mv_int, error);
    }
    printf("\n\n");

    printf("## Fixed Linear Conversion (AVR 8-bit e.g. atmega328p)\n\n");
    printf("| ADC | Millivolt | ADC back | error |\n");
    printf("|-----|-----------|----------|-------|\n");
    for (int i = 0; i <= (1 << ADC_BIT_COUNT); i += 64)                                                                                                                     \
    {
        uint8_t adc_val = (int)i;
        uint8_t millivolt_fixed = ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, adc_val);
        uint8_t adc_from_mv_fixed = ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(SCALING_FACTOR, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, millivolt_fixed);
        int error = (int)adc_from_mv_fixed - (int)adc_val;
        printf("| %3u | %9u | %8u | %5d |\n", adc_val, millivolt_fixed, adc_from_mv_fixed, error);
    }
    printf("\n\n");

    printf("## Floating Linear Conversion (AVR 32-bit Soft Float float e.g. atmega328p)\n\n");
    printf("### ADC Value --> Millivolt --> ADC Value \n\n");
    printf("| ADC Value  | Millivolts (float) | ADC From mV (int) | error  |\n");
    printf("|------------|--------------------|-------------------|--------|\n");
    for (int i = 0; i <= (1 << ADC_BIT_COUNT); i += 64)
    {
        float adc_val = (float)i;
        float millivolt_float = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, (float)MILLI_VOLT_REFERENCE, (float)adc_val);
        uint16_t adc_from_mv_int = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, millivolt_float);
        printf("| %10f | %15.2f mV | %17u | %6d |\n", adc_val, millivolt_float, adc_from_mv_int, adc_from_mv_int - adc_val);
    }
    printf("\n\n");

    printf("### Millivolt --> ADC Value --> Millivolt \n\n");
    printf("| MV Value | ADC From mV (int) | Millivolts (float) | error  |\n");
    printf("|----------|-------------------|--------------------|--------|\n");
    for (int mv_val = 0; mv_val <= MILLI_VOLT_REFERENCE; mv_val += 100)
    {
        uint16_t adc_from_mv_int = ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, mv_val);
        float millivolt_float = ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, (float)MILLI_VOLT_REFERENCE, (float)adc_from_mv_int);
        printf("| %5u mV | %17u | %15.2f mV | %6.2f |\n", mv_val, adc_from_mv_int, millivolt_float, millivolt_float - (float)mv_val);
    }
    printf("\n\n");

	// this quits the simulator, since interrupts are off
	// this is a "feature" that allows running tests cases and exit
	sleep_cpu();
}
