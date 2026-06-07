#include <stdio.h>
#include <string.h>

//create a file  that uses the password needed

int main()
{
    char input[100];

    printf("Password: ");
    scanf("%99s", input);

    if (strcmp(input, "1234") == 0) {
        printf("Access granted\n");
    } else {
        printf("Access denied\n");
    }

    return 0;
}