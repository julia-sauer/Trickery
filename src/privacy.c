#define _GNU_SOURCE
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

int open(const char *path, int flag, ...) {
    printf("hijacked functions for open got called!\n");
    int(*real_open)(const char *, int, ...) = dlsym(RTLD_NEXT, "open");

    return real_open(path, flag);
}

// remove hijack (rm in terminal)   zum teschte: LD_PRELOAD=./privacy.so rm cannotRemove.txt
int unlinkat(int dirfd, const char *pathname, int flags) {
    printf("Hijacked unlinkat() called!\n");
    printf("You do not have permission to delete: %s\n", pathname);
    
    errno = EACCES; // "Permission denied"
    return -1; // signals failure to the caller
}

// cat hijack   zum teschte: LD_PRELOAD=./privacy.so rm cannotRemove.txt
ssize_t write(int fildes, const void *buf, size_t nbyte) {
    static __thread int active = 0; // flag so there won't be a loop

    if (fildes == 1 && !active) {
        active = 1;
        printf("Hijacked write() called!\n");
        const char* msg = "I don't feel like doing something right now...\n";
        syscall(SYS_write, fildes, msg, strlen(msg)); // syscall calls kernel directly -> maybe necessary for all printf statements if there are problems
        active = 0;
        return nbyte;
    }

    return syscall(SYS_write, fildes, buf, nbyte); // if it's no write to terminal (fildes != 1) then do the intended thing
}
