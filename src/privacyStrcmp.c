#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

//create a pointer to function to store address of real libc
static int (*real_strcmp)(const char *, const char *) = NULL;
static int inside_strcmp_hook = 0; //recursion controll --> if 0 you are not in the hook

//check blocked in config.txt
int is_blocked_password(const char *password)
{
    FILE *config = fopen("config.txt", "r");

    if (config == NULL) {
        return 0;
    }

    char line[256];

    while (fgets(line, sizeof(line), config)) {

        if (strncmp(line, "BLOCK_PASSWORD=", 15) == 0) {

            char *blocked = line + 15; //pointer to superSecretPassword0000
            blocked[strcspn(blocked, "\r\n")] = '\0';

            if (strcmp(password, blocked) == 0) {
                fclose(config);
                return 1;
            }
        }
    }

    fclose(config);
    return 0;
}

// how to test:     STEP1: LD_PRELOAD=./src/privacy.so ./passwordNeeded
//                  STEP2: when password asked write "superSecretPassword0000"
// strcmp hijacking --> every password denied (even the right one)
int strcmp(const char *s1, const char *s2)
{
    if (real_strcmp == NULL) {
        real_strcmp = dlsym(RTLD_NEXT, "strcmp");
    }

     if (inside_strcmp_hook) {
            return real_strcmp(s1, s2);
        }

    inside_strcmp_hook = 1;

    if (is_blocked_password(s2)) {
        const char *msg =
            "here we go again, why not use a sticky note next time?\n";

        syscall(SYS_write, 2, msg, strlen(msg));
        inside_strcmp_hook = 0;
        return 1;   // force "not equal"
    }

    inside_strcmp_hook = 0;
    return real_strcmp(s1, s2);
}