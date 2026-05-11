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
    fprintf(fptr, "Hello, this is a sample text written to the file.\n");
    fprintf(fptr, "File handling in C is simple and efficient.\n");

    fclose(fptr); // Close the file

    printf("Data successfully written to the file.\n");
    return 0;
}