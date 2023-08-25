#include "imu_algebra.h"

////////////////////////////////////////////


imu_vec3_t imu_vec3_create(float x, float y, float z)
{
    imu_vec3_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}


////////////////////////////////////////////


imu_vec3_t imu_vec3_sum(const imu_vec3_t * v1, const imu_vec3_t * v2)
{
    return imu_vec3_create(v1->x + v2->x, v1->y + v2->y, v1->z + v2->z);
}


////////////////////////////////////////////


imu_vec3_t imu_vec3_dif(const imu_vec3_t * v1, const imu_vec3_t * v2)
{
    return imu_vec3_create(v1->x - v2->x, v1->y - v2->y, v1->z - v2->z);
}


////////////////////////////////////////////


imu_vec3_t imu_vec3_divide(const imu_vec3_t * v1, const imu_vec3_t * v2)
{
    return imu_vec3_create(v1->x / v2->x, v1->y / v2->y, v1->z / v2->z);
}


////////////////////////////////////////////


float imu_vec3_dot(const imu_vec3_t * v1, const imu_vec3_t * v2)
{
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}


////////////////////////////////////////////


imu_vec3_t imu_vec3_cross(const imu_vec3_t * v1, const imu_vec3_t * v2)
{
    return imu_vec3_create(
        v1->y * v2->z - v1->z * v2->y,
        - v1->x * v2->z + v1->z * v2->x,
        v1->x * v2->y - v2->x * v1->y
    );
}


////////////////////////////////////////////


imu_vec3_t imu_vec3_normalize(const imu_vec3_t * v)
{
    float multiplier = imu_math_fast_inv_sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    return imu_vec3_create(v->x * multiplier, v->y * multiplier, v->z * multiplier);
}


////////////////////////////////////////////


imu_vec3_t imu_vec3_scale(const imu_vec3_t * q, float multiplier)
{
    return imu_vec3_create(q->x * multiplier, q->y * multiplier, q->z * multiplier);
}


////////////////////////////////////////////


float imu_vec3_length(const imu_vec3_t * v)
{
    return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_create(float w, float x, float y, float z)
{
    imu_quaternion_t q;
    q.w = w;
    q.x = x;
    q.y = y;
    q.z = z;
    return q;
}


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_sum(const imu_quaternion_t * q1, const imu_quaternion_t * q2)
{
    return imu_quaternion_create(q1->w + q2->w, q1->x + q2->x, q1->y + q2->y, q1->z + q2->z);
}


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_product(const imu_quaternion_t * q1, const imu_quaternion_t * q2)
{
    return imu_quaternion_create(
        (q1->w*q2->w) - (q1->x*q2->x) - (q1->y*q2->y) - (q1->z*q2->z),
        (q1->w*q2->x) + (q1->x*q2->w) + (q1->y*q2->z) - (q1->z*q2->y),
        (q1->w*q2->y) - (q1->x*q2->z) + (q1->y*q2->w) + (q1->z*q2->x),
        (q1->w*q2->z) + (q1->x*q2->y) - (q1->y*q2->x) + (q1->z*q2->w)
    );
}


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_conjugate(const imu_quaternion_t * q)
{
    return imu_quaternion_create(q->w, - q->x, - q->y, - q->z);
}


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_inverse(const imu_quaternion_t * q)
{
    imu_quaternion_t qcjg = imu_quaternion_conjugate(q);
    float qcjg_len = imu_quaternion_length(&qcjg);
    return imu_quaternion_scale(&qcjg, 1.f / (qcjg_len * qcjg_len));
}


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_normalize(const imu_quaternion_t * q)
{
    float multiplier = imu_math_fast_inv_sqrt(q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z);
    return imu_quaternion_create(q->w * multiplier, q->x * multiplier, q->y * multiplier, q->z * multiplier);
}


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_scale(const imu_quaternion_t * q, float multiplier)
{
    return imu_quaternion_create(q->w * multiplier, q->x * multiplier, q->y * multiplier, q->z * multiplier);
}


////////////////////////////////////////////


imu_vec3_t imu_quaternion_rotate_vector(const imu_quaternion_t * q, imu_vec3_t * v)
{
    imu_quaternion_t qv = imu_quaternion_create(0., v->x, v->y, v->z);
    imu_quaternion_t qinv = imu_quaternion_conjugate(&qv);
    imu_quaternion_t tmp = imu_quaternion_product(q, &qv);
    imu_quaternion_t vrot = imu_quaternion_product(&tmp, &qinv);
    return imu_vec3_create(vrot.x, vrot.y, vrot.z);
}


////////////////////////////////////////////


imu_euler_t imu_quaternion_to_euler(const imu_quaternion_t * q)
{
    imu_euler_t e;
    e.roll = atan2(2 * (q->w* q->x + q->y * q->z), 1 - 2 * (q->x * q->x + q->y * q->y));
    e.pitch = asin(2 * (q->w * q->y - q->z * q->x));
    e.yaw = q->z == 0 ? 0.0 : atan2(2 * (q->w * q->z + q->x * q->y), 1- 2 * (q->y * q->y + q->z * q->z));
    return e;
}


////////////////////////////////////////////


float imu_quaternion_length(const imu_quaternion_t * q)
{
    return sqrt(q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z);
}


////////////////////////////////////////////


imu_quaternion_t imu_quaternion_rotate_vector_quaternion(const imu_quaternion_t * q, const imu_quaternion_t * qu)
{
    imu_quaternion_t q1 = imu_quaternion_product(q, qu);
    imu_quaternion_t q2 = imu_quaternion_inverse(q);
    return imu_quaternion_product(&q1, &q2);
}


////////////////////////////////////////////