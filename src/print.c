#include <stdio.h>
#include <stdarg.h>

int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    puts("D Jana stinkt unglaublich schlimm.");
    //vprintf(format, args); // would print original also
    va_end(args);
    return 0;
}