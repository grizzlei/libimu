## libimu v0.1

_Quaternion based IMU data processing library._

 ![libimu-demo](meta/resources/libimu-demo.gif)

_Disclaimer: I'm not an OpenGL expert, forgive me for using legacy api calls and other bad practices in `demo.c`._

### Example use
Here's a simplified piece of code from `demo.c`. Following code is essentially all you need to compute the current orientation of the body.

```c
imu_t imu = imu_init();
// alternatively you can set IMU_CALIBMODE_NEVER (default) or IMU_CALIBMODE_PERIODIC
imu_set_calibration_mode(&imu, IMU_CALIBMODE_ONCE); 
// let's say we are using a popular imu that is mpu6050
// and it is initialized with ±500 °/s gyro, ±4g accelerometer configurations
// then the raw data from sensors will be multiplied with below values per mpu6050 datasheet
imu_set_gyro_scale_factor(&imu, 2.f/131.f);
imu_set_accelerometer_scale_factor(&imu, 2.f/16384.f);

// application loop
// ..

    // fill ax, ay, az and gx, gy, gz from sensors

    imu_set_accelerometer_raw(&imu, ax, ay, az);
    imu_set_gyro_raw(&imu, gx, gy, gz);
    imu_main_loop(&imu);

    // get orientation either in euler angles or as a quaternion as below

    prdbg("euler angles: %f %f %f", imu.orientation.roll, imu.orientation.pitch, imu.orientation.yaw);
    prdbg("quaternion: %f %f %f %f", imu.orientation_quat.w, imu.orientation_quat.x, imu.orientation_quat.y, imu.orientation_quat.z);

// ..
// end of application loop

```

In application loop, `ax, ay, az` and `gx, gy, gz` have to be acquired from accelerometer and gyro sensors.

---

### Dependencies for `demo.c`

```
apt-get install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev libglfw3-dev
```

---

### MPU6050 tool for Orange Pi and Raspberry Pi boards

In `demo.c` to acquire sensor data, i've used a tool I wrote to use mpu6050 with OrangePi Zero board. You can get the tool and wiring information from https://github.com/grizzlei/mpu6050.

If you wish to use another tool, please remember that expected format is: `ax,ay,az,gx,gy,gz\r\n` over serial with 115200 bps (8N1).

---

### Thanks

Thanks to Gordon Wetzstein for the content and Stanford University for making them available online. Math behind the library essentially relies on this lecture note.

https://stanford.edu/class/ee267/lectures/lecture10.pdf

---

### License

_libimu is a free software, it can be used by anyone who try to build a better world._

MIT License

Copyright (c) 2022 Hasan Karaman

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

* * *

[hasan karaman (whoami)](https://hasankaraman.dev/whoami) - [https://hasankaraman.dev](https://hasankaraman.dev) - [hk@hasankaraman.dev](mailto:hk@hasankaraman.dev) - 2022