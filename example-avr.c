/*
 * example-avr.c
 * AVR unit tests for linmap - run under simavr
 *
 * Uses simavr's command-register mechanism (SIMAVR_CMD_EXIT_CODE_0/1) so
 * simavr exits with code 0 on pass or 1 on failure.  CI can check $? directly.
 *
 * Key bugs exposed by this test on AVR (sizeof(int)==2):
 *  - Plain int overflow: (512 * 3300) = 1,689,600 overflows int16_t.
 *    Fix: use int32_t/long literals, e.g. LINMAP_X_TO_Y(0, 0L, 1023, 3300L, x).
 *  - Fixed-point overflow at SCALE=10: (1023 * 3300) << 10 = 3,456,921,600
 *    exceeds INT32_MAX. Use SCALE<=8 for 10-bit ADC + 3300 mV, or widen to uint32_t.
 */

#include "adc_linmap.h"
#include "linmap.h"
#include "simavr_cmd.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <stdio.h>

/* Declare GPIOR0 as the simavr command bridge (unused general-purpose IO reg) */
SIMAVR_DECLARE_CMD_REG(&GPIOR0);

/* 10-bit ADC, 3.3 V reference (matches desktop example) */
#define ADC_BIT_COUNT   10
#define MILLI_VOLT_REF  3300L

/* SCALE=8:  max intermediate = (1023 * 3300) << 8  =   864,230,400 < INT32_MAX -- safe  */
/* SCALE=10: max intermediate = (1023 * 3300) << 10 = 3,456,921,600 > INT32_MAX -- overflow */
#define SCALE_SAFE       8
#define SCALE_OVERFLOW  10

/* ---- UART --------------------------------------------------------------- */
static void uart_init(void)
{
    UBRR0 = 103; /* 16 MHz / 16 / 9600 - 1 */
    UCSR0B = (1 << TXEN0);
}

static int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream);
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

static FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

/* ---- Test framework ---------------------------------------------------- */
static int failures = 0;

static void check(const char *name, int32_t got, int32_t expected, int32_t tol)
{
    int32_t err = got - expected;
    if (err < 0)
        err = -err;
    if (err <= tol)
        printf("PASS: %s\n", name);
    else {
        printf("FAIL: %s  got=%ld expected=%ld\n", name, (long)got, (long)expected);
        ++failures;
    }
}

/* ---- Main -------------------------------------------------------------- */
int main(void)
{
    stdout = &uart_output;
    uart_init();

    printf("# linmap AVR unit tests\n\n");
    printf("sizeof(int)=%u  sizeof(long)=%u\n\n",
           (unsigned)sizeof(int), (unsigned)sizeof(long));

    /* --- Basic correctness: explicit int32_t (long on AVR) --------------- */
    check("x_to_y zero",
          LINMAP_X_TO_Y(0, (int32_t)0, 1023, (int32_t)3300, (int32_t)0), 0, 0);
    check("x_to_y mid",
          LINMAP_X_TO_Y(0, (int32_t)0, 1023, (int32_t)3300, (int32_t)512), 1651, 1);
    check("x_to_y full",
          LINMAP_X_TO_Y(0, (int32_t)0, 1023, (int32_t)3300, (int32_t)1023), 3300, 0);

    check("y_to_x zero",
          LINMAP_Y_TO_X(0, (int32_t)0, 1023, (int32_t)3300, (int32_t)0), 0, 0);
    check("y_to_x mid",
          LINMAP_Y_TO_X(0, (int32_t)0, 1023, (int32_t)3300, (int32_t)1651), 512, 1);
    check("y_to_x full",
          LINMAP_Y_TO_X(0, (int32_t)0, 1023, (int32_t)3300, (int32_t)3300), 1023, 0);

    /* --- Fixed-point, SCALE=8 (safe for 10-bit ADC + 3300 mV) ----------- */
    check("fp x_to_y mid  SCALE=8",
          LINMAP_X_TO_Y_FIXED_POINT(SCALE_SAFE, 0, (int32_t)0, 1023, (int32_t)3300, (int32_t)512), 1651, 1);
    check("fp x_to_y full SCALE=8",
          LINMAP_X_TO_Y_FIXED_POINT(SCALE_SAFE, 0, (int32_t)0, 1023, (int32_t)3300, (int32_t)1023), 3300, 1);
    check("fp y_to_x mid  SCALE=8",
          LINMAP_Y_TO_X_FIXED_POINT(SCALE_SAFE, 0, (int32_t)0, 1023, (int32_t)3300, (int32_t)1651), 512, 1);

    /* --- Round-trip via adc_linmap.h helpers ----------------------------- */
    {
        int32_t adc_in = 300;
        int32_t mv     = ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(SCALE_SAFE, ADC_BIT_COUNT, MILLI_VOLT_REF, adc_in);
        int32_t adc_rt = ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(SCALE_SAFE, ADC_BIT_COUNT, MILLI_VOLT_REF, mv);
        check("round-trip adc->mv->adc SCALE=8", adc_rt, adc_in, 1);
    }

    /* --- Bug: fixed-point overflow at SCALE=10, max ADC value ------------ */
    /* (1023 * 3300) << 10 = 3,456,921,600 overflows int32_t (max 2,147,483,647).
     * Fix: use SCALE<=8, or widen intermediate to uint32_t/int64_t. */
    check("fp x_to_y full SCALE=10 (int32_t overflow)",
          LINMAP_X_TO_Y_FIXED_POINT(SCALE_OVERFLOW, 0, (int32_t)0, 1023, (int32_t)3300, (int32_t)1023), 3300, 1);

    /* --- Bug: plain int is 16-bit on AVR; 512*3300 overflows ------------- */
    /* Fix: always use L or int32_t literals with ADC-scale values, e.g.
     *   LINMAP_X_TO_Y(0, 0L, 1023, 3300L, (int32_t)x)                    */
    check("plain int x_to_y mid (int16 overflow on AVR)",
          LINMAP_X_TO_Y(0, 0, 1023, 3300, 512), 1651, 1);

    /* --- Demo table (for README) ---------------------------------------- */
    printf("\n## Fixed-point conversion table (SCALE=%d, 10-bit ADC, 3300 mV ref)\n\n",
           SCALE_SAFE);
    printf("| ADC | mV (fp) | ADC back | error |\n");
    printf("|-----|---------|----------|-------|\n");
    for (int32_t i = 0; i <= (1 << ADC_BIT_COUNT); i += 64)
    {
        int32_t mv  = ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(SCALE_SAFE, ADC_BIT_COUNT, MILLI_VOLT_REF, i);
        int32_t rt  = ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(SCALE_SAFE, ADC_BIT_COUNT, MILLI_VOLT_REF, mv);
        printf("| %3ld | %7ld | %8ld | %5ld |\n", (long)i, (long)mv, (long)rt, (long)(rt - i));
    }

    printf("\n## Summary: %d failure(s)\n", failures);

    /* Signal pass/fail to simavr via the command register, then halt.
     * simavr exits with code 0 (EXIT_CODE_0) or 1 (EXIT_CODE_1) immediately
     * on the register write; sleep_cpu() is the fallback for older simavr. */
    GPIOR0 = failures ? SIMAVR_CMD_EXIT_CODE_1 : SIMAVR_CMD_EXIT_CODE_0;
    sleep_cpu();
    return 0;
}
