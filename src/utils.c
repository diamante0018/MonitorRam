#include <stdarg.h>
#include <stdio.h>

#include "utils.h"

#define VA_BUFFER_COUNT 4
#define VA_BUFFER_SIZE 4096

struct va_info_s {
  char va_string[VA_BUFFER_COUNT][VA_BUFFER_SIZE];
  int index;
};

static __thread struct va_info_s va_provider;

char* va(const char* fmt, ...) {
  va_list ap;

  int index = va_provider.index;
  va_provider.index = (va_provider.index + 1) % VA_BUFFER_COUNT;
  char* buf = va_provider.va_string[index];

  va_start(ap, fmt);
  vsnprintf(buf, VA_BUFFER_SIZE, fmt, ap);
  va_end(ap);

  buf[VA_BUFFER_SIZE - 1] = '\0';

  return buf;
}
