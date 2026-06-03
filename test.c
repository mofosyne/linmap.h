#include "adc_linmap.h"
#include "linmap.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

void test_linmap_x_to_y(void)
{
    // Basic doubling: [0, 100] -> [0, 200]
    assert(LINMAP_X_TO_Y(0, 0, 100, 200, 0) == 0);
    assert(LINMAP_X_TO_Y(0, 0, 100, 200, 50) == 100);
    assert(LINMAP_X_TO_Y(0, 0, 100, 200, 100) == 200);

    // Offset output range: [0, 10] -> [10, 20]
    assert(LINMAP_X_TO_Y(0, 10, 10, 20, 0) == 10);
    assert(LINMAP_X_TO_Y(0, 10, 10, 20, 5) == 15);
    assert(LINMAP_X_TO_Y(0, 10, 10, 20, 10) == 20);

    // Identity: [0, 100] -> [0, 100]
    assert(LINMAP_X_TO_Y(0, 0, 100, 100, 42) == 42);

    // Inverted output: [0, 100] -> [100, 0]
    assert(LINMAP_X_TO_Y(0, 100, 100, 0, 0) == 100);
    assert(LINMAP_X_TO_Y(0, 100, 100, 0, 50) == 50);
    assert(LINMAP_X_TO_Y(0, 100, 100, 0, 100) == 0);

    // Integer truncation: result rounds toward zero (standard C behavior)
    // Exact: 1/3 * 10 = 3.33 -> 3
    assert(LINMAP_X_TO_Y(0, 0, 3, 10, 1) == 3);

    printf("LINMAP_X_TO_Y tests passed!\n");
}

void test_linmap_y_to_x(void)
{
    // Inverse of doubling: [0, 100] -> [0, 200]
    assert(LINMAP_Y_TO_X(0, 0, 100, 200, 0) == 0);
    assert(LINMAP_Y_TO_X(0, 0, 100, 200, 100) == 50);
    assert(LINMAP_Y_TO_X(0, 0, 100, 200, 200) == 100);

    // Inverse of offset: [0, 10] -> [10, 20]
    assert(LINMAP_Y_TO_X(0, 10, 10, 20, 10) == 0);
    assert(LINMAP_Y_TO_X(0, 10, 10, 20, 15) == 5);
    assert(LINMAP_Y_TO_X(0, 10, 10, 20, 20) == 10);

    printf("LINMAP_Y_TO_X tests passed!\n");
}

void test_linmap_negative_values(void)
{
    // Negative input range: [-100, 100] -> [0, 200]
    assert(LINMAP_X_TO_Y(-100, 0, 100, 200, -100) == 0);
    assert(LINMAP_X_TO_Y(-100, 0, 100, 200, 0) == 100);
    assert(LINMAP_X_TO_Y(-100, 0, 100, 200, 100) == 200);

    // Negative output range: [0, 100] -> [-100, 100]
    assert(LINMAP_X_TO_Y(0, -100, 100, 100, 0) == -100);
    assert(LINMAP_X_TO_Y(0, -100, 100, 100, 50) == 0);
    assert(LINMAP_X_TO_Y(0, -100, 100, 100, 100) == 100);

    // Inverse with negative input range
    assert(LINMAP_Y_TO_X(-100, 0, 100, 200, 0) == -100);
    assert(LINMAP_Y_TO_X(-100, 0, 100, 200, 100) == 0);
    assert(LINMAP_Y_TO_X(-100, 0, 100, 200, 200) == 100);

    // Both ranges negative: [-200, -100] -> [-50, -25]
    // X=-200 -> Y=-50 (range start)
    assert(LINMAP_X_TO_Y(-200, -50, -100, -25, -200) == -50);
    // X=-150: C evaluates Y1 + (dX*dY)/dXrange = -50 + (50*25)/100 = -50 + 12 = -38
    // (integer division truncates 12.5 to 12 before adding to Y1)
    assert(LINMAP_X_TO_Y(-200, -50, -100, -25, -150) == -38);
    // X=-100 -> Y=-25 (range end)
    assert(LINMAP_X_TO_Y(-200, -50, -100, -25, -100) == -25);

    printf("Negative value tests passed!\n");
}

void test_linmap_fixed_point(void)
{
    // Exact mappings should be identical to plain integer version
    assert(LINMAP_X_TO_Y_FIXED_POINT(8, 0, 0, 100, 200, 0) == 0);
    assert(LINMAP_X_TO_Y_FIXED_POINT(8, 0, 0, 100, 200, 50) == 100);
    assert(LINMAP_X_TO_Y_FIXED_POINT(8, 0, 0, 100, 200, 100) == 200);

    assert(LINMAP_Y_TO_X_FIXED_POINT(8, 0, 0, 100, 200, 0) == 0);
    assert(LINMAP_Y_TO_X_FIXED_POINT(8, 0, 0, 100, 200, 100) == 50);
    assert(LINMAP_Y_TO_X_FIXED_POINT(8, 0, 0, 100, 200, 200) == 100);

    // Fixed-point precision: scale=8 uses 256x internal resolution
    // Plain: (1 * 10) / 3 = 3 (truncated from 3.33)
    // Fixed: ((1 * 10 * 256) / 3) >> 8 = 2560/3 >> 8 = 853 >> 8 = 3
    assert(LINMAP_X_TO_Y(0, 0, 3, 10, 1) == 3);
    assert(LINMAP_X_TO_Y_FIXED_POINT(8, 0, 0, 3, 10, 1) == 3);

    printf("Fixed-point tests passed!\n");
}

void test_adc_linmap(void)
{
    // 10-bit ADC, 3300 mV reference: range [0, 1023] <-> [0, 3300]
    assert(ADC_MILLIVOLT_FROM_VAL(10, 3300L, 0) == 0);
    assert(ADC_MILLIVOLT_FROM_VAL(10, 3300L, 1023) == 3300);

    assert(ADC_VAL_FROM_MILLIVOLT(10, 3300L, 0) == 0);
    assert(ADC_VAL_FROM_MILLIVOLT(10, 3300L, 3300) == 1023);

    // Fixed-point ADC conversion boundary checks
    assert(ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(10, 10, 3300L, 0) == 0);
    assert(ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(10, 10, 3300L, 1023) == 3300);

    assert(ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(10, 10, 3300L, 0) == 0);
    assert(ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(10, 10, 3300L, 3300) == 1023);

    printf("ADC linmap tests passed!\n");
}

int main(void)
{
    test_linmap_x_to_y();
    test_linmap_y_to_x();
    test_linmap_negative_values();
    test_linmap_fixed_point();
    test_adc_linmap();

    printf("All tests passed successfully!\n");
    return 0;
}
