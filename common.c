#include <time.h>
#include <unistd.h>

#include "common.h"

// Prints the time
void get_time_stamp(char* buf) {
  time_t ltime;
  struct tm result;

  ltime = time(NULL);
  localtime_r(&ltime, &result);
  asctime_r(&result, buf);
}
