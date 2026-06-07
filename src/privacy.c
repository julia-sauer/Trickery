#define _GNU_SOURCE
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <stdarg.h>

// global var to see if for write blocked file got opened
static int block_write_output = 0;

int config_block_words(const char *path, const char *mode) {
    FILE *(*real_fopen)(const char *restrict, const char *restrict) = dlsym(RTLD_NEXT, "fopen");
    int (*real_fclose)(FILE *) = dlsym(RTLD_NEXT, "fclose");

    FILE *config = real_fopen("config.txt", "r"); //opens the config file with read permission
    if(config == NULL){
        return 0;
    }
    char line[256];

    while (fgets(line, sizeof(line), config)) {
        if(strncmp(line, "BLOCK_OPEN=", 11) == 0 && strcmp(mode, "BLOCK_OPEN") == 0) { //first part is to compare the actual path with the words in the config file, second part is to make sure that successful comparisons only happen with the relevant BLOCK of the config file
            char *blockedWord = line + 11;
            blockedWord[strcspn(blockedWord, "\r\n")] = '\0';

            if(strstr(path, blockedWord) != NULL) { //strstr searches for part strings so if the path has somewhere this blockedWord string this is true
                real_fclose(config);
                return 1;
            }
        }
        
        if(strncmp(line, "BLOCK_DELETE=", 13) == 0 && strcmp(mode, "BLOCK_DELETE") == 0) {
            char *blockedWord = line + 13;
            blockedWord[strcspn(blockedWord, "\r\n")] = '\0';

            if(strstr(path, blockedWord) != NULL) { //strstr searches for part strings so if the path has somewhere this blockedWord string this is true
                real_fclose(config);
                return 1;
            }
        }

        if (strncmp(line, "BLOCK_MOVE=", 11) == 0 && strcmp(mode, "BLOCK_MOVE") == 0) {
            char *blockedWord = line + 11;
            blockedWord[strcspn(blockedWord, "\r\n")] = '\0';

            if (strstr(path, blockedWord) != NULL) {
                real_fclose(config);
                return 1;
            }
        }

        if(strncmp(line, "BLOCK_READ=", 11) == 0 && strcmp(mode, "BLOCK_READ") == 0) {
            char *blockedWord = line + 11;
            blockedWord[strcspn(blockedWord, "\r\n")] = '\0';

            if(strlen(blockedWord) > 0 && strstr(path, blockedWord) != NULL) {
                real_fclose(config);
                return 1;
            }
        }

        if(strncmp(line, "BLOCK_WRITE_FILE=", 17) == 0 && strcmp(mode, "BLOCK_WRITE_FILE") == 0) {
            char *blockedWord = line + 17;
            blockedWord[strcspn(blockedWord, "\r\n")] = '\0';

            if(strstr(path, blockedWord) != NULL) { //strstr searches for part strings so if the path has somewhere this blockedWord string this is true
                real_fclose(config);
                return 1;
            }
        }

        if(strncmp(line, "BLOCK_WRITE_CAT=", 16) == 0 && strcmp(mode, "BLOCK_WRITE_CAT") == 0) {
            char *blockedWord = line + 16;
            blockedWord[strcspn(blockedWord, "\r\n")] = '\0';

            if(strstr(path, blockedWord) != NULL) { //strstr searches for part strings so if the path has somewhere this blockedWord string this is true
                real_fclose(config);
                return 1;
            }
        }

        if(strncmp(line, "BLOCK_CLOSE=", 12) == 0 && strcmp(mode, "BLOCK_CLOSE") == 0) {
            char *blockedWord = line + 12;
            blockedWord[strcspn(blockedWord, "\r\n")] = '\0';

            if(strlen(blockedWord) > 0 && strstr(path, blockedWord) != NULL) {
                real_fclose(config);
                return 1;
            }       
        }
    }
    real_fclose(config);
    return 0;
}

// open hijack to test: cat
int open(const char *path, int flag, ...) {
    syscall(SYS_write, 1, "Hijacked function for open got called!\n", strlen("Hijacked function for open got called!\n"));
    if(config_block_words(path, "BLOCK_OPEN")) {
        printf("[hook] access denied: %s\n", path);
        errno = EACCES;
        return -1;
    }
    int(*real_open)(const char *, int, ...) = dlsym(RTLD_NEXT, "open");

    if (config_block_words(path, "BLOCK_WRITE_CAT")) {
        block_write_output = 1;
    } else {
        block_write_output = 0;
    }

    return real_open(path, flag);
}

// remove hijack (rm in terminal)   to test: LD_PRELOAD=./privacy.so rm 02-OS-FS26-Project-Topics.pdf
int unlinkat(int dirfd, const char *pathname, int flags) {
    syscall(SYS_write, 1, "Hijacked unlinkat() called!\n", strlen("Hijacked unlinkat() called!\n"));
    if(config_block_words(pathname, "BLOCK_DELETE")) {
        printf("[hook] access denied: %s\n", pathname);
        errno = EACCES; // "Permission denied"
        return -1;
    }
    printf("[hook] access granted: %s\n", pathname);
    int(*real_unlinkat)(int, const char *, int) = dlsym(RTLD_NEXT, "unlinkat");

    return real_unlinkat(dirfd, pathname, flags);
}

// write hijack for writing into terminal (cat)  to test: LD_PRELOAD=./privacy.so cat ./tests/readTest.c
ssize_t write(int fildes, const void *buf, size_t nbyte) {
    static int shown = 0;
    static __thread int active = 0;

    if (fildes == 1 && !active && !shown) { // hijack for the write function into the terminal
        active = 1;
        shown = 1; 

        const char* hijack = "Hijacked write() called!\n";
        syscall(SYS_write, fildes, hijack, strlen(hijack));

        const char* art =
        ".................:::::;+xxxxxxx+++xx++++++++++x+++++++;;;;;;;;;;;;;;;;;+;;;\n"
        "..................::;xXxxxxXXxxxxxxxxxxxxxxxxxxxxxx+++++++;;;;;;;;;;;;;+;;;\n"
        "...............:::;xxxxxxxXXXxxxxxx++++++++xXXXXXXxxxxx++++++;;;;;;;;;;+;;;\n"
        "..............::;+xxxxxxx+++++++++;++;;;;;;;;++xXXXXXxxxx+++++;;;;;;;;;+;;;\n"
        "..........:::::+xxxxxx++++++;;;+;;;;;;;;;;;;;;;;++xXXXXXxx+++++++;;;;;;+;;;\n"
        "......:::::::;+xxxxxx+++++;;;;;;;;;;;;;;;;:;;;;;;;;+xxXxxxx+++++;;;;;;;+;;;\n"
        "..::::::::::;++xXxxx++;;;;;;;;;;;;;;;;;;:::::::::;;;+++xxxxx+++;;;;;;;;+;;;\n"
        ":::::::::::;+xxXxx++++;;;;;;;;;;;;;;;;;:::::::::::::;;++xxxx++++;;;;;;;+;;;\n"
        ":::::::::;+++xXxx++;;;;;;;;;;;;;;;;;;;;::::::::::::::;;;+++xx++++;;;;;;+;;;\n"
        "::::::::;+xxxXx++;;;;;;;;;;;;;;;;;;;;;;::::::::::::::::;;;+x+x+++++;;;;+;;;\n"
        ":::::::;+xxXXx+;;;;;;;;;;;;;;;;;;;;;;;;;:::::::::::::::::;;+x+++++;;;;;+;;;\n"
        ":::::::+xxXXx+;::::::;;;+++++;+;;;;;;;;;;;:::::::::::::::::;+++++++;;;;+;;;\n"
        "::::::;xxxXX+;::::::;;;+++++++++++;;+;+;;;;;;;;:::::::::::::;+++;;+;;;;+;;;\n"
        "::::::+xxXXx;::.:::;;++++++++++++++++++++++++;;;;;:::::::::::;;+;;;;;;;;;;;\n"
        ":::::;+xxXX+::.::;;;+++++++xx+++++++++++++++++++;;;;:::::::.:;+;;;;;;;;;;;;\n"
        ":::::;x+XXX;::;;+++++xxxxxxxxxxx++++++++xxxxxxxxx++++;;;;:::.;+;;;;;;;;;;;;\n"
        ":::::++xXXx:;;++xxxXXXXXXXxxxxxxx++++++xxxxXXXXXXXxxx+++;;;:.:+x++;;;;;;;;;\n"
        "::::;xxXX$XxXXXXXXXXXXXXXXXXXxxxx++++++xxxXXXXXXXXXXX$$$XXX$XXXXXXx++;;;;;;\n"
        "$$$&$Xxxxxx++xxxxxxX$$$XXXXXXXxx+++;;;;+xxXXX$$&$Xx+++++++++;;++;;x$&&&$X;;\n"
        "&&$x:::++xX+xxxx++++;++xX$&$XXxx++;;:;;;+x$&$XXxx++;;;;+++++;+x+;:;+X&&&$;;\n"
        "&&$XxxxxxxX+xxxxxxx++++xXXXXX$$$Xx+++xX$&$XXXXXXXxx+++++++++;;xXxXxxX$&&$+;\n"
        "&$XXXXx$$$$+xxXXXxxxxXXX$$$XXX&&&&&&$&&&xxXX$$$$$XXxx++xx+++++X$XXX$x$&$+;;\n"
        "$$+:.:;xx$$+xxXXXX$$$XxxxXXXXX$&&&&$&&&$+;;xX$$$$$$$$XxxXXx+;+$$X;::+X$;::;\n"
        ";$+:::;+;:+++x+xxXXXXXXxxxXXXx&&X;;::;&&x;++xXXXXXxxxx+++++;;++++;;:x$+;;::\n"
        ":xx::;;;;++++++xxxxxXxxxxxxXxX&$+;;:::+&$+;+xXXXXXXXXxx++;::;+++;;;;$X;:;;:\n"
        ":;$+::;;:;;;+++++++++++xxxxxx&&X+;;:::;x&X+;+xxXXxxxx++;;:;;+;+;;;;x$;;::;:\n"
        "::;X;::;;;;;;++++++++++++++x$$Xx;;;::::;X$X+;;+++++++;;;;++++;;;;;x$;:;;::;\n"
        ":::;X;:::;;;;++++;;;;;;;+++$$Xx+;;;:::::+xXX+;;;;;;;;;;;++xx+;;;;xX;:::;:::\n"
        ":::::+X+:;;;++++;;;;;;+++x$xxx++;;;::::::x+X$+;;;;;;;;;;;+++;;;XX+;;::;;;;;\n"
        "::::::;;$&$x++++++;;++x$&X+xx+;;;;;;:::::;x++X&Xx+;;;;;;;;+X$X++x+;;::;;;;;\n"
        "::::::;;Xx+++xxxxxxx++;++xxxx+;;;;;;::::::xxxx++++++++++;;;;;;;+Xx;;;;;;;;;\n"
        ":::;::;xXX+++++;;;;;;++xxxXx+;;;;;;;;:::::;xXXxxx++++;;;;;;;;;;XXX+;;;;;;;;\n"
        "::;::;;xXX++++++++++++xxXXXx++;;;;;;;;;:::;+XXXXxxx+++++;;;;;;+XXXXx;;;;;;;\n"
        ":;;:;;+xXXX+++++++++xxxXXXxx+++;;+;;;;;;;;;+xXXXXxxxxx++;;;;;;xXXxXX+;;;;;;\n"
        "++;;;;+XXXX++++++++xxxXXXxxxxx++++++;;;;++xxxxXXXXXxxxx++;;;;+XXXxXXx+;;;;;\n"
        ";;;;;+xX$$XX+++++xxxxxXXxxxxxxxxxxxxxxxxx++xxxxXXXXXxxx++++++X$XXXxxXx+;;;;\n"
        "+++;;+XX$$XXx+++xxxxXXxx+xxxxxxxxXXXXXXx;;;;++xxXXXXXxxxx+++xXXX$XXXXx+;;;;\n"
        ":;+;+xX$$$$XXx++xxxxxxxxxx+++xxxxxXXXx+;;::;;;++xxxXxxx+++++xxxXXXXXXXx;;;;\n"
        ";;+;+XX$$$$X$X++xxxxxxxxxxx+xxxxxXXXxx+;;;;;;;+xxxxxxx++++++xxXxxXxxXXx+;;+\n"
        "+;++xX$$$$$X$$x++xxxxxxXXxxxxxxxxxxx++xx+++xxXXXXXxx+++++;+XXxxxxxxx++++++;\n"
        "xXxXXX$$$$$XXX$x++xxxxXXXxxXXXX$$$$XXX$XXXx+;;;;++++++++++X$$XxxxXx++++++;;\n"
        "+xXXX$$$X$$XX$$&xxxxxxxxxxxxxxXXXXXxxx+++;;;;;;;;;+++++++X$$$$XXXxXXxxxxx++\n"
        "xxxXX$$$XXXXX$&&$xxxxxxxxxxxxxxxxxx+++++++;;;;;;;;;++++x$&$$$$$$XXXXXXx++++\n"
        "$XXX$$$$X$$X$&&&&&Xxxxxxxxxxxxxxxxxxxxxxx+++;;;;;;;;++x$&&$&$&&&$$$$$X$Xx++\n"
        "xXXX$$$XX$XX$&&&&&&$xxxxxxxxxxxxxxxxxxxx+++;;;;;;;;;+$&&&&$&$&&&&$$X$$$$XXX\n"
        "XXX$$$$X$XXX$&&&&&&&&Xx++xxxxxxxxx++++++;;;;;;;;;;;x$&&&&$$&$&&$&$$XX$$$$$$\n"
        "XXX$$$$X$$$$&&&&&&&&&&$xxx+++++++++++;;;;;;:;:;;;+X&&&&&$$$$$&&$$$$$$$$$$$$\n"
        "$XX$$$$$$$$$&&&&&&&&&&&&Xx++++;;;;;;;;;;;;;;;;;;+$&&&&&&$&$$&&&$$$$$$$$$$$$\n"
        "XX$$$$$$$$$&&&&&&&&&&&&&&$xx++++;;;;;;;;;;;;;;+x&&&$&&&&$$$&&&&$$$$$$$$$$$$\n"
        "X$$$$$$X$$$&&&&&&&&&&&&&&&&Xxx++++++++++;;;;++$&&&&&&&$$$$&&&&&$&$$$$$$$$$$\n"
        "$$$$$$X$&$$$&&&&&&&&&&&&&&&&$Xxxxxxxxxxx++xx$&&&&&&&&$$$&&&&&&&$&&$$$$&$$$$\n"
        "$$$$$X$&$$$&&&&&&&&&&&&&&&&&&&&$$XXXXXXX$$&&&&&&&&&&&&&&&&&&&&&&&&$$$$&&&$$\n"
        "$XXXX$$$$$&&$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$$&&&&&&&&&&&&&$$$$&&&&&\n"
        "XXX$$$$$$$&&$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$$&&&&&&&&&&&&&&&&&&$$$&&&&&&\n"
        "XX$$$$$$$$&&&$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$$&&&&&&&&&&&&&&&&&&&&$$&&&&&&\n";
        syscall(SYS_write, 1, art, strlen(art));

        const char* msg = "Big Jenna is watching you...\n";
        syscall(SYS_write, fildes, msg, strlen(msg)); // syscall calls kernel directly -> maybe necessary for all printf statements if there are problems

        active = 0;
        return nbyte;
    }

    return syscall(SYS_write, fildes, buf, nbyte); // if it's no write to terminal (fildes != 1) then do the intended thing
}

// write hijack for writing into files   zum teschte: LD_PRELOAD=./privacy.so ./writeTest
size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream) {

    size_t(*real_fwrite)(const void *, size_t, size_t, FILE *) = dlsym(RTLD_NEXT, "fwrite");

    if (!real_fwrite) {
        return 0;
    }

    const char *dbg = "Hijacked fwrite() called!\n"; // debug message to terminal (stderr)
    write(2, dbg, strlen(dbg));

    char fd_path[64];
    char real_path[512];
    int fd = fileno(stream); // converts FILE* to a file descriptor
    snprintf(fd_path, sizeof(fd_path), "/proc/self/fd/%d", fd); // builds path link
    ssize_t len = readlink(fd_path, real_path, sizeof(real_path) - 1); // builds real path out of path link

    if (len != -1){
        real_path[len] = '\0';
        if(config_block_words(real_path, "BLOCK_WRITE_FILE")) {
            const char *msg = "HIJACKED FILE CONTENT: NO 'CHEAT' WHATSOEVER ALLOWED!!\n";
            real_fwrite(msg, 1, strlen(msg), stream);
            return nitems;
        }
    }    

    real_fwrite(ptr, size, nitems, stream);
    return nitems;
}

// read hijack for reading files. To test: LD_PRELOAD=./libpriv.so ./readTest
// Expected output: READ RETURNED THIS: Nothing to see here. The real content has been hidden.
ssize_t read(int fd, void *buf, size_t count) {
    syscall(SYS_write, 1, "Hijacked read() called!\n", strlen("Hijacked read() called!\n"));
    static ssize_t (*real_read)(int, void *, size_t) = NULL;

    if (!real_read) {
        real_read = dlsym(RTLD_NEXT, "read");
    }

    char fd_path[64];
    char real_path[512];

    snprintf(fd_path, sizeof(fd_path), "/proc/self/fd/%d", fd);

    ssize_t len = readlink(fd_path, real_path, sizeof(real_path) - 1);
    if (len != -1) {
        real_path[len] = '\0';

        if (config_block_words(real_path, "BLOCK_READ")) {
            static int fake_already_sent = 0;

            if(fake_already_sent){
                fake_already_sent = 0;
                return 0;
            }

            const char *fake = "Nothing to see here. The real content has been hidden.\n";
            size_t fake_len = strlen(fake);

            if (count < fake_len) {
                fake_len = count;
            }

            memcpy(buf, fake, fake_len);
            fake_already_sent = 1;
            return fake_len;
        }
    }

    return real_read(fd, buf, count);
}

//hook for rename() -->testing mv (since mv also uses renameat() with wsl and renameat2() on VM, also wrote a hook for them)
//to test it: LD_PRELOAD=./privacy.so mv "filename1" "filename2"
int rename(const char *oldpath, const char *newpath)
{
    printf("Hijacked rename() called!\n");
    static int (*real_rename)(const char *, const char *) = NULL;

    if (real_rename == NULL) {
        real_rename = dlsym(RTLD_NEXT, "rename");
    }

    if (config_block_words(oldpath, "BLOCK_MOVE")) {
        printf("rename denied: %s important file can't be renamed\n", oldpath);
        errno = EACCES;
        return -1;
    }

    printf("you got fooled hehe! new filename changed to: you_wish\n");

    return real_rename(oldpath, "you_wish.txt");
}

int renameat(int olddirfd, const char *oldpath,
             int newdirfd, const char *newpath)
{
    printf("Hijacked renameat() called!\n");
    static int (*real_renameat)(int, const char *, int, const char *) = NULL;

    if (real_renameat == NULL) {
        real_renameat = dlsym(RTLD_NEXT, "renameat");
    }

    if (config_block_words(oldpath, "BLOCK_MOVE")) {
        printf("renameing denied: %s important file can't be renamed\n", oldpath);
        errno = EACCES;
        return -1;
    }

    printf("you got fooled hehe! new filename changed to: you_wish\n");

    return real_renameat(olddirfd, oldpath, newdirfd, "you_wish.txt");
}

int renameat2(int olddirfd, const char *oldpath,
              int newdirfd, const char *newpath,
              unsigned int flags)
{
    fprintf(stderr, "Hijacked renameat2() called!\n");

    static int (*real_renameat2)(int, const char *, int, const char *, unsigned int) = NULL;

    if (real_renameat2 == NULL) {
        real_renameat2 = dlsym(RTLD_NEXT, "renameat2");
    }

    if (config_block_words(oldpath, "BLOCK_MOVE")) {
        fprintf(stderr, "renaming denied: %s important file can't be renamed\n", oldpath);
        errno = EACCES;
        return -1;
    }

    fprintf(stderr, "you got fooled hehe! new filename changed to: you_wish.txt\n");

    return real_renameat2(olddirfd, oldpath, newdirfd, "you_wish.txt", flags);
}

// fclose hijack: asking user three times before closing important files.
// To test: LD_PRELOAD=./libpriv.so ./fcloseTest
int fclose(FILE *stream) {
    //printf("Hijacked fclose() called!\n");
    static int (*real_fclose)(FILE *) = NULL;
    static __thread int active = 0;

    if (!real_fclose) {
        real_fclose = dlsym(RTLD_NEXT, "fclose");
    }

    if (active) {
        return real_fclose(stream);
    }

    active = 1;

    char fd_path[64];
    char real_path[512];
    int fd = fileno(stream);

    snprintf(fd_path, sizeof(fd_path), "/proc/self/fd/%d", fd);

    ssize_t len = readlink(fd_path, real_path, sizeof(real_path) - 1);
    if (len != -1) {
        real_path[len] = '\0';

        if (config_block_words(real_path, "BLOCK_CLOSE")) {
            char answer[16];

            for (int i = 0; i < 3; i++) {
                printf("Are you sure you want to close this file? [y/N]: ");
                fflush(stdout);

                if (fgets(answer, sizeof(answer), stdin) == NULL || answer[0] != 'y') {
                    active = 0;
                    errno = EACCES;
                    return EOF;
                }
            }
        }
    }

    active = 0;
    return real_fclose(stream);
}

//fopen function for using it with a c program
FILE *fopen(const char *restrict path, const char *restrict mode) {
    static __thread int active = 0;

    FILE *(*real_fopen)(const char *restrict, const char *restrict) = dlsym(RTLD_NEXT, "fopen");

    if(active) {
        return real_fopen(path, mode);
    }
    active = 1;

    //syscall(SYS_write, 1, "Hijacked fopen() called!\n", strlen("Hijacked fopen() called!\n"));

    if(config_block_words(path, "BLOCK_OPEN")) {
        printf("[hook] access denied by fopen: %s\n", path);
        active = 0;
        errno = EACCES;
        return NULL;
    }

    if (config_block_words(path, "BLOCK_WRITE_CAT")) {
        block_write_output = 1;
    } else {
        block_write_output = 0;
    }

    active = 0;
    return real_fopen(path, mode);
}
