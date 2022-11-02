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

#ifndef IMU_ALGEBRA_H
#define IMU_ALGEBRA_H

#include "imu_math.h"
#include "imu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////


imu_vec3_t imu_vec3_create(float x, float y, float z);


////////////////////////////////////////////


imu_vec3_t imu_vec3_sum(const imu_vec3_t * v1, const imu_vec3_t * v2);


////////////////////////////////////////////


imu_vec3_t imu_vec3_dif(const imu_vec3_t * v1, const imu_vec3_t * v2);


////////////////////////////////////////////


imu_vec3_t imu_vec3_divide(const imu_vec3_t * v1, const imu_vec3_t * v2);


////////////////////////////////////////////


imu_vec3_t imu_vec3_normalize(const imu_vec3_t * v);


////////////////////////////////////////////


imu_vec3_t imu_vec3_scale(const imu_vec3_t * v, float multiplier);


////////////////////////////////////////////


float imu_vec3_length(const imu_vec3_t * v);


////////////////////////////////////////////


float imu_vec3_dot(const imu_vec3_t * v1, const imu_vec3_t * v2);


////////////////////////////////////////////


imu_vec3_t imu_vec3_cross(const imu_vec3_t * v1, const imu_vec3_t * v2);

////////////////////////////////////////////


imu_quaternion_t imu_quaternion_create(float w, float x, float y, float z);


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_sum(const imu_quaternion_t * q1, const imu_quaternion_t * q2);


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_product(const imu_quaternion_t * q1, const imu_quaternion_t * q2);


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_conjugate(const imu_quaternion_t * q);


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_inverse(const imu_quaternion_t * q);


////////////////////////////////////////////


float * imu_quaternion_to_rotation_mat(const imu_quaternion_t * q);


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_normalize(const imu_quaternion_t * q);


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_scale(const imu_quaternion_t * q, float multiplier);


////////////////////////////////////////////


imu_vec3_t imu_quaternion_rotate_vector(const imu_quaternion_t * q, imu_vec3_t * v);


////////////////////////////////////////////


imu_euler_t imu_quaternion_to_euler(const imu_quaternion_t * q);


////////////////////////////////////////////


float imu_quaternion_length(const imu_quaternion_t * q);


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_rotate_vector_quaternion(const imu_quaternion_t * q, const imu_quaternion_t * qu);


////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif