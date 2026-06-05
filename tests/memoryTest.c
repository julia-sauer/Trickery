#include <stdlib.h>

int main() {
    malloc(100);

    char *p = malloc(50);
    free(p);

    return 0;
}