/**
 * MIT License
 * Copyright (c) 2022 Hasan Karaman (github.com/grizzlei)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

#ifndef IMU_MATH_H
#define IMU_MATH_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////

#define PI 3.14159265359

#define d2r(x)(x * PI / 180)
#define r2d(x)(x * 180 / PI)


////////////////////////////////////////////


/// fast inverse sqrt algorithm from Quake III Arena source (copy-paste).
float imu_math_fast_inv_sqrt(float n);


////////////////////////////////////////////


float imu_math_map_value(float value, float min, float max, float mapped_min, float mapped_max);


////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif