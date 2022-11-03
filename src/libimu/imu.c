#include "imu.h"

////////////////////////////////////////////


int16_t imu_priv_calib_counter = 0;
int16_t const IMU_PRIV_CALIB_COUNTER_MAX = 200;

imu_vec3_t imu_priv_gyro_movavg_buf[30] = {0};
int8_t imu_priv_gyro_movavg_idx = 0;


////////////////////////////////////////////


imu_t imu_init()
{
    imu_t imu;

    imu_set_state(&imu, IMU_STATE_UNCALIBRATED);
    imu_set_calibration_mode(&imu, IMU_CALIBMODE_NEVER);
    
    imu.accelerometer_offset =
        imu.gyro_offset =
            imu.accelerometer =
                imu.gyro = imu_vec3_create(0.f, 0.f, 0.f);

    imu.accelerometer_raw = imu_vec3_create(0.f, 0.f, 0.f);
    imu.gyro_raw = imu_vec3_create(0.f, 0.f, 0.f);
    
    imu.orientation_quat = imu_quaternion_create(1.f, 0.f, 0.f, 0.f);
    imu._gyro_ts = get_time_sec();

    return imu;
}


////////////////////////////////////////////


void imu_calibrate(imu_t *imu)
{
    if(imu_priv_calib_counter++ < IMU_PRIV_CALIB_COUNTER_MAX)
    {
        imu->gyro_offset.x += imu->gyro_raw.x / IMU_PRIV_CALIB_COUNTER_MAX;
        imu->gyro_offset.y += imu->gyro_raw.y / IMU_PRIV_CALIB_COUNTER_MAX;
        imu->gyro_offset.z += imu->gyro_raw.z / IMU_PRIV_CALIB_COUNTER_MAX;
    }
    else
    {
        imu_priv_calib_counter = 0;
        imu_set_state(imu, IMU_STATE_READY);
    }
}


////////////////////////////////////////////


void imu_main_loop(imu_t *imu)
{
    switch (imu->state)
    {
    case IMU_STATE_UNCALIBRATED:

        if(imu->_calibration_mode != IMU_CALIBMODE_NEVER)
        {
            imu->_calibration_time = time(NULL);
            imu_set_state(imu, IMU_STATE_CALIBRATING);
            imu->gyro_offset = imu->accelerometer_offset = imu_vec3_create(0.f, 0.f, 0.f);
        }
        else
        {
            imu_set_state(imu, IMU_STATE_READY);
            prwar("calibration mode set to IMU_CALIBMODE_NEVER. if you need it, call imu_set_calibration_mode with IMU_CALIBMODE_ONCE or IMU_CALIBMODE_PERIODIC.");
        }
        break;

    case IMU_STATE_CALIBRATING:

        imu_calibrate(imu);
        break;

    case IMU_STATE_READY:

        imu_process_raw_data(imu);

        if(imu->_calibration_mode == IMU_CALIBMODE_PERIODIC)
        {
            if(time(NULL) - imu->_calibration_time > IMU_CALIBRATION_PERIOD)
            {
                imu_set_state(imu, IMU_STATE_UNCALIBRATED);
            }
        }

        break;

    default:
        break;
    }
}


////////////////////////////////////////////


void imu_set_state(imu_t * imu, int state)
{
    imu->state = state;
}


////////////////////////////////////////////


void imu_set_gyro_raw(imu_t * imu, float gx, float gy, float gz)
{
    imu->gyro_raw.x = gx;
    imu->gyro_raw.y = gy;
    imu->gyro_raw.z = gz;
}


////////////////////////////////////////////


void imu_set_accelerometer_raw(imu_t * imu, float ax, float ay, float az)
{
    imu->accelerometer_raw.x = ax;
    imu->accelerometer_raw.y = ay;
    imu->accelerometer_raw.z = az;
}


////////////////////////////////////////////


void imu_set_estimation_mode(imu_t * imu, int8_t mode)
{
    imu->_estimation_mode = mode;
}


////////////////////////////////////////////


void imu_set_calibration_mode(imu_t * imu, int8_t mode)
{
    imu->_calibration_mode = mode;
}


////////////////////////////////////////////


int8_t imu_get_estimation_mode(imu_t * imu)
{
    return imu->_estimation_mode;
}


////////////////////////////////////////////


void imu_process_raw_data(imu_t * imu)
{
    // subtracting mean noise offsets from new raw values
    imu->accelerometer = imu_vec3_dif(&imu->accelerometer_raw, &imu->accelerometer_offset);
    imu->gyro = imu_vec3_dif(&imu->gyro_raw, &imu->gyro_offset);

    // scaling corrected raw data to 
    imu->accelerometer = imu_vec3_scale(&imu->accelerometer, imu->_scale_factor_accelerometer);
    imu->gyro = imu_vec3_scale(&imu->gyro, imu->_scale_factor_gyro);
    
    const float alpha = 0.96f, one_minus_alpha = (1.f - alpha);

    ////////////////////////////////////////////
    // gyro integration
    ////////////////////////////////////////////

    float dtime = get_time_sec() - imu->_gyro_ts;
    float rotvlen = imu_vec3_length(&imu->gyro);
    float rotang = d2r(dtime * rotvlen);
    float crotang_2 = cos(rotang * 0.5);
    float srotang_2 = sin(rotang * 0.5);

    imu_vec3_t rotn = imu_vec3_normalize(&imu->gyro);
    // instantaneous rotation quaternion
    imu_quaternion_t rotation = imu_quaternion_create(crotang_2, rotn.x * srotang_2, rotn.y * srotang_2, rotn.z * srotang_2);
    // integrated gyro quaternion
    imu_quaternion_t qw = imu_quaternion_product(&imu->orientation_quat, &rotation);

    imu->_gyro_ts = get_time_sec();

    ////////////////////////////////////////////
    // complementary filter
    ////////////////////////////////////////////

    // gravity vector quaternion in body coordinates
    imu_quaternion_t qabody = imu_quaternion_create(0.f ,imu->accelerometer.x, imu->accelerometer.y, imu->accelerometer.z);
    // current estimation of world space
    imu_quaternion_t qawrld = imu_quaternion_rotate_vector_quaternion(&qw, &qabody);
    // normalize qaworld
    qawrld = imu_quaternion_normalize(&qawrld);
    // up vector of world
    imu_vec3_t wup = imu_vec3_create(0.f, 0.f, 1.f);
    imu_vec3_t v = imu_vec3_create(qawrld.x, qawrld.y, qawrld.z);
    imu_vec3_t n = imu_vec3_cross(&v, &wup);
    n = imu_vec3_normalize(&n);
    float tiltang = acos(imu_vec3_dot(&v, &wup)) * one_minus_alpha;
    float ctiltang_2 = cos(tiltang * 0.5);
    float stiltang_2 = sin(tiltang * 0.5);
    // tilt correction quaternion
    imu_quaternion_t qt = imu_quaternion_create(ctiltang_2, n.x * stiltang_2, n.y * stiltang_2, n.z * stiltang_2);
    // resulting quaternion of complementary filter
    imu->orientation_quat = imu_quaternion_product(&qt, &qw);
    // updating orientation
    imu->orientation = imu_quaternion_to_euler(&imu->orientation_quat);
}


////////////////////////////////////////////


void imu_set_gyro_scale_factor(imu_t * imu, float scalefactor)
{
    imu->_scale_factor_gyro = scalefactor;
}


////////////////////////////////////////////


void imu_set_accelerometer_scale_factor(imu_t * imu, float scalefactor)
{
    imu->_scale_factor_accelerometer = scalefactor;
}


////////////////////////////////////////////