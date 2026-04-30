#define _GNU_SOURCE
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>

int unlinkat(int dirfd, const char *pathname, int flags) {
    printf("Hijacked unlinkat() called!\n");
    printf("You do not have permission to delete: %s\n", pathname);
    
    errno = EACCES;   // "Permission denied"
    return -1;        // signals failure to the caller
}