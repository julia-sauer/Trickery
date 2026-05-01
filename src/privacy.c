#define _GNU_SOURCE
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>

int open(const char *path, int flag, ...) {
    printf("hijacked functions for open got called!\n");
    int(*real_open)(const char *, int, ...) = dlsym(RTLD_NEXT, "open");

    return real_open(path, flag);
}

// remove hijack (rm in terminal)
int unlinkat(int dirfd, const char *pathname, int flags) {
    printf("Hijacked unlinkat() called!\n");
    printf("You do not have permission to delete: %s\n", pathname);
    
    errno = EACCES;   // "Permission denied"
    return -1;        // signals failure to the caller
}