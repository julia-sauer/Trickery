#include <stdio.h>

int main() {
    FILE *file = fopen("important.txt", "r");

    if (file == NULL) {
        perror("fopen failed");
        return 1;
    }

    printf("File was opened successfully.\n");
    printf("Now trying to close the file...\n");

    if (fclose(file) == 0) {
        printf("File was closed successfully.\n");
    } else {
        perror("fclose failed");
    }

    return 0;
}