#include <stdio.h>
#include <stdarg.h>
#include <threads.h>

#include "utils.h"

#define VA_BUFFER_COUNT 4
#define VA_BUFFER_SIZE 4096

thread_local struct {
  char va_string[VA_BUFFER_COUNT][VA_BUFFER_SIZE];
  int index;
} va_info_t;

char* va(const char* fmt, ...) {
  va_list ap;

  int index = va_info_t.index;
  va_info_t.index = (va_info_t.index + 1) % VA_BUFFER_COUNT;
  char* buf = va_info_t.va_string[index];

  va_start(ap, fmt);
  vsnprintf(buf, VA_BUFFER_SIZE, fmt, ap);
  va_end(ap);

  buf[VA_BUFFER_SIZE - 1] = '\0';

  return buf;
}
