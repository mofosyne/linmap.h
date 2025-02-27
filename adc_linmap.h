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

#define ADC_MILLIVOLT_FROM_VAL(ADC_BIT_COUNT, MILLI_VOLT_REFL, MILLI_VOLT_REFH, ADC_VALUE) LINMAP_X_TO_Y(0, MILLI_VOLT_REFL, (1 << (ADC_BIT_COUNT)) - 1, MILLI_VOLT_REFH, ADC_VALUE)
#define ADC_VAL_FROM_MILLIVOLT(ADC_BIT_COUNT, MILLI_VOLT_REFL, MILLI_VOLT_REFH, MILLIVOLT) LINMAP_Y_TO_X(0, MILLI_VOLT_REFL, (1 << (ADC_BIT_COUNT)) - 1, MILLI_VOLT_REFH, MILLIVOLT)

#define ADC_MILLIVOLT_FROM_VAL_FIXED_POINT(SCALE, ADC_BIT_COUNT, MILLI_VOLT_REFL, MILLI_VOLT_REFH, ADC_VALUE)                                                                                          \
    LINMAP_X_TO_Y_FIXED_POINT(SCALE, 0, MILLI_VOLT_REFL, (1 << (ADC_BIT_COUNT)) - 1, MILLI_VOLT_REFH, ADC_VALUE)

#define ADC_VAL_FROM_MILLIVOLT_FIXED_POINT(SCALE, ADC_BIT_COUNT, MILLI_VOLT_REFL, MILLI_VOLT_REFH, MILLIVOLT)                                                                                          \
    LINMAP_Y_TO_X_FIXED_POINT(SCALE, 0, MILLI_VOLT_REFL, (1 << (ADC_BIT_COUNT)) - 1, MILLI_VOLT_REFH, MILLIVOLT)

#endif
