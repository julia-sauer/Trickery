#define _GNU_SOURCE
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>

int open(const char *path, int flag, ...) {
    printf("hijacked functions for open got called!\n");
    int(*real_open)(const char *, int, ...) = dlsym(RTLD_NEXT, "open");

    return real_open(path, flag);
}