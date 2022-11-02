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

#ifndef IMU_CONSTANTS_H
#define IMU_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////


#define IMU_STATE_UNCALIBRATED      0x01
#define IMU_STATE_CALIBRATING       0x02
#define IMU_STATE_READY             0x04

#define IMU_CALIBMODE_NEVER         0x00
#define IMU_CALIBMODE_PERIODIC      0x01
#define IMU_CALIBMODE_ONCE          0x02

#define IMU_ESTIMODE_GYRO           0x01
#define IMU_ESTIMODE_ACCELEROMETER  0x02
#define IMU_ESTIMODE_MAGNETOMETER   0x04

#define IMU_CALIBRATION_BUFLEN      0x3C
#define IMU_CALIBRATION_PERIOD      0x14 // seconds
#define IMU_CALIBRATION_DURATION    0x05
#define IMU_UNINITIALIZED           0x98967F   


////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif