#define _GNU_SOUCRE
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>

int open(const char *path, int flag, ...) {
    fprintf(stdout, "hijacked functions for openat got called!");
    int(*real_open)(const char *, int, ...) = dlsym(RTLD_NEXT, "open");

    return real_open(path, flag);
}