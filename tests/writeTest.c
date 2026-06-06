#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// test program for writing into files
int main() {
    
    FILE *fptr; // Declare a file pointer

    fptr = fopen("output.txt", "w"); // Open the file in write mode

    if (fptr == NULL) { // Check if the file was opened successfully
        printf("Error! Unable to open file.\n");
        exit(1); // Exit the program if the file cannot be opened
    }

    // Write data to the file
    fprintf(fptr, "Hello, this is just some ordinary output text.\n");
    fprintf(fptr, "This works? Noice.\n");

    fclose(fptr); // Close the file

    printf("Data successfully written to the file.\n");

    
    FILE *fptr2; // Declare a file pointer

    fptr2 = fopen("cheatsheet_OS.txt", "w"); // Open the file in write mode

    if (fptr2 == NULL) { // Check if the file was opened successfully
        printf("Error! Unable to open file.\n");
        exit(1); // Exit the program if the file cannot be opened
    }

    // Write data to the file
    fprintf(fptr2, "Help me pass this exam.\n");
    fprintf(fptr2, "Here is everything you need to know for the exam: 0_0\n");

    fclose(fptr2); // Close the file

    printf("Data successfully written to the file.\n");
    return 0;
}