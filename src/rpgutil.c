#include "common.h"
#include "rpg.h"

void print_cur_time()
{
    struct timeval now;
    struct timezone tz;
    struct tm *tm;

    gettimeofday(&now, &tz);
    tm = localtime(&now.tv_sec);
    fprintf(stdout, "%.2d:%.2d:%.2d.%.6ld \n", tm->tm_hour, tm->tm_min, tm->tm_sec, now.tv_usec);
}
