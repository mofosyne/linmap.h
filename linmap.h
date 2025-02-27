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

#ifndef LINMAP_H
#define LINMAP_H

// This is the linear conversion macros
#define LINMAP_X_TO_Y(X1, Y1, X2, Y2, X) ((Y1) + (((X) - (X1)) * ((Y2) - (Y1))) / ((X2) - (X1)))
#define LINMAP_Y_TO_X(X1, Y1, X2, Y2, Y) ((X1) + (((Y) - (Y1)) * ((X2) - (X1))) / ((Y2) - (Y1)))

// This uses left shift (Akin to scaling down via multiplication) to temporarily convert the integer division into fixed integer arithmetic operation
// before using right shift (Akin to scaling upward via division) to convert from fixed notation back to the original scale but with better precision
// Reference: https://en.wikipedia.org/wiki/Fixed-point_arithmetic
#define LINMAP_X_TO_Y_FIXED_POINT(SCALE, X1, Y1, X2, Y2, X) ((((Y1) << (SCALE)) + ((((X) - (X1)) * ((Y2) - (Y1))) << (SCALE)) / ((X2) - (X1))) >> (SCALE))
#define LINMAP_Y_TO_X_FIXED_POINT(SCALE, X1, Y1, X2, Y2, Y) ((((X1) << (SCALE)) + ((((Y) - (Y1)) * ((X2) - (X1))) << (SCALE)) / ((Y2) - (Y1))) >> (SCALE))

#endif
