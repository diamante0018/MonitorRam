#include "common.h"
#include "utils.h"

#define VA_BUFFER_COUNT 4
#define VA_BUFFER_SIZE 4096

char *va(const char *fmt, ...)
{
    static char g_vaBuffer[VA_BUFFER_COUNT][VA_BUFFER_SIZE];
    static int g_vaNextBufferIndex = 0;

    va_list ap;
    char *dest = &g_vaBuffer[g_vaNextBufferIndex][0];
    bzero(dest, VA_BUFFER_SIZE);
    g_vaNextBufferIndex = (g_vaNextBufferIndex + 1) % VA_BUFFER_COUNT;
    va_start(ap, fmt);
    vsnprintf(dest, VA_BUFFER_SIZE, fmt, ap);
    va_end(ap);
    dest[VA_BUFFER_SIZE - 1] = '\0';

    return dest;
}
