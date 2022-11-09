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

#ifndef IMU_H
#define IMU_H

#include <stdint.h>

#include "imu_types.h"
#include "imu_utils.h"
#include "imu_algebra.h"
#include "imu_constants.h"

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////


extern int16_t imu_priv_calib_counter;
extern int16_t const IMU_PRIV_CALIB_COUNTER_MAX;

////////////////////////////////////////////


typedef struct IMU 
{
    // processed gyro data
    imu_vec3_t gyro;

    // processed accelerometer data
    imu_vec3_t accelerometer;
    
    // gyro calibration offsets
    // we will subtract these offset values from every imu->gyro_raw in imu_main_loop()
    imu_vec3_t gyro_offset;
    
    // if we need gravity vector of calibration epoch in sensor frame coordinates we'll use these.
    // it's basically the average gravity vector from calibration.
    // these are not necessarily offset values.
    // for sake of consistency in naming I call them offset.
    imu_vec3_t accelerometer_offset;
    
    // raw gyro data. SET THIS USING imu_set_gyro_raw()
    imu_vec3_t gyro_raw;
    
    // raw accelerometer data. SET THIS USING imu_set_accelerometer_raw()
    imu_vec3_t accelerometer_raw;

    // computed orientation quaternion of the body
    imu_quaternion_t orientation_quat;

    // orientation of the body in roll, pitch and yaw angles
    imu_euler_t orientation;
    
    // current computational state of the library.
    int8_t state;
    
    // timestamp to compute angular change in gyro
    double _gyro_ts;
    
    // number to multiply raw gyro data. changes according to full scale
    double _scale_factor_gyro;

    // number to multiply raw accelerometer data changes according to full scale
    double _scale_factor_accelerometer;

    // timestamp of calibration change if status is calibrating, won't be updated. if status is ready imu will be recalibrated every n seconds.
    time_t _calibration_time;

    // IMU_CALIBMODE_NEVER, IMU_CALIBMODE_ONCE or IMU_CALIBMODE_PERIODIC
    int8_t _calibration_mode;
    
    // flags: IMU_ESTIMODE_GYRO, IMU_ESTIMODE_ACCELEROMETER or IMU_ESTIMODE_MAGNETOMETER (functionality disabled for now)
    int8_t _estimation_mode;

} imu_t;


////////////////////////////////////////////


void imu_main_loop(imu_t * imu);


////////////////////////////////////////////


imu_t imu_init();


////////////////////////////////////////////


void imu_calibrate(imu_t * imu);


////////////////////////////////////////////


void imu_set_state(imu_t * imu, int state);


////////////////////////////////////////////


void imu_set_gyro_raw(imu_t * imu, float gx, float gy, float gz);


////////////////////////////////////////////


void imu_set_accelerometer_raw(imu_t * imu, float ax, float ay, float az);


////////////////////////////////////////////


void imu_set_estimation_mode(imu_t * imu, int8_t mode);


////////////////////////////////////////////


void imu_set_calibration_mode(imu_t * imu, int8_t mode);


////////////////////////////////////////////


int8_t imu_get_estimation_mode(imu_t * imu);


////////////////////////////////////////////


void imu_process_raw_data(imu_t * imu);


////////////////////////////////////////////


void imu_set_gyro_scale_factor(imu_t * imu, float scalefactor);


////////////////////////////////////////////


void imu_set_accelerometer_scale_factor(imu_t * imu, float scalefactor);


////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif