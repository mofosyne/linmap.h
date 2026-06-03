/*
    Linear Conversion/Mapping/Interpolation Macros
    Author: Brian Khuu 2025

    # MIT License

    Copyright (c) 2025 Brian Khuu

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#ifndef ADC_LINMAP_H
#define ADC_LINMAP_H

#include "linmap.h"

/*
 * Fast ADC conversion macros (recommended for embedded use)
 *
 * Uses (1 << ADC_BIT_COUNT) as the full-scale divisor, which the compiler
 * reduces to a right-shift -- no software division needed.
 *
 * Error vs the exact (1<<N)-1 divisor: < 0.13% for a 10-bit ADC (< 4 mV
 * at 3.3 V), which is well within ADC noise.
 *
 * On AVR, these avoid the ~100-cycle software division routine.
 * Use int32_t arguments on platforms where sizeof(int)==2.
 */
#define ADC_MILLIVOLT_FROM_VAL_FAST(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, ADC_VALUE) \
    (((int32_t)(ADC_VALUE) * (int32_t)(MILLI_VOLT_REFERENCE)) >> (ADC_BIT_COUNT))

#define ADC_VAL_FROM_MILLIVOLT_FAST(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, MILLIVOLT) \
    (((int32_t)(MILLIVOLT) << (ADC_BIT_COUNT)) / (int32_t)(MILLI_VOLT_REFERENCE))

/*
 * Exact ADC conversion macros (uses (1<<N)-1 as full-scale)
 *
 * Mathematically exact: ADC=2^N-1 maps to exactly MILLI_VOLT_REFERENCE.
 * Requires a software integer division on every call (~100 cycles on AVR).
 * The _FIXED_POINT variants below are kept for compatibility but produce
 * identical results to the plain variants when X1=Y1=0 (which is always
 * true for ADC); prefer the _FAST macros above for new code.
 */
#define ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, ADC_VALUE) \
    LINMAP_X_TO_Y(0, 0L, (1 << (ADC_BIT_COUNT)) - 1, MILLI_VOLT_REFERENCE, ADC_VALUE)

#define ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, MILLIVOLT) \
    LINMAP_Y_TO_X(0, 0L, (1 << (ADC_BIT_COUNT)) - 1, MILLI_VOLT_REFERENCE, MILLIVOLT)

#define ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(SCALE, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, ADC_VALUE) \
    LINMAP_X_TO_Y_FIXED_POINT(SCALE, 0, 0L, (1 << (ADC_BIT_COUNT)) - 1, MILLI_VOLT_REFERENCE, ADC_VALUE)

#define ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(SCALE, ADC_BIT_COUNT, MILLI_VOLT_REFERENCE, MILLIVOLT) \
    LINMAP_Y_TO_X_FIXED_POINT(SCALE, 0, 0L, (1 << (ADC_BIT_COUNT)) - 1, MILLI_VOLT_REFERENCE, MILLIVOLT)

#endif
