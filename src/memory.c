#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void *malloc(size_t size) {
    //fprintf(stderr, "Hijacked function malloc called!\n");
    write(STDOUT_FILENO, "Hijacked function malloc called!\n", strlen("Hijacked function malloc called!\n"));
    void *(*real_malloc)(size_t) = dlsym(RTLD_NEXT, "malloc");

    return (*real_malloc)(size);
    
}

void free(void *ptr) {
    write(STDOUT_FILENO, "Hijacked function free called!\n", strlen("Hijacked function free called!\n"));
    void (*real_free)(void*) = dlsym(RTLD_NEXT, "free");

    return real_free(ptr);
}
