#include "imu_utils.h"


////////////////////////////////////////////


double get_time_sec()
{
    #if defined(__linux__)
        struct timespec now = {0, 0};
        clock_gettime(CLOCK_MONOTONIC, &now);
        return (double)now.tv_sec + 1e-9 * now.tv_nsec;
    #endif
}


////////////////////////////////////////////