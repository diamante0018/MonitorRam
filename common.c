#include "common.h"

// Prints the time
void printTimeStamp(FILE *fp)
{
    time_t ltime;
    struct tm result;
    char stime[32];
    bzero(stime, sizeof(stime));

    ltime = time(NULL);
    localtime_r(&ltime, &result);
    asctime_r(&result, stime);
    fprintf(fp, "\nTimeStamp: %s\n", stime);
}
