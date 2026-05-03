#define _GNU_SOURCE
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

int config_block_words(const char *path) {
    FILE *config = fopen("config.txt", "r"); //opens the config file with read permission
    if(config == NULL){
        return 0;
    }
    char line[256];

    while (fgets(line, sizeof(line), config)) {
        if(strncmp(line, "BLOCK_OPEN=", 11) == 0) {
            char *blockedWord = line + 11;
            blockedWord[strcspn(blockedWord, "\n")] = '\0';

            if(strstr(path, blockedWord) != NULL) {
                fclose;
                return 1;
            }
        }
    }
    fclose;
    return 0;

}

// open hijack zum teschte: cat
int open(const char *path, int flag, ...) {
    printf("hijacked functions for open got called!\n");
    if(config_block_words(path)) {
        printf("[hook] access denied: %s\n", path);
        errno = EACCES;
        return -1;
    }
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

// write hijack   zum teschte: LD_PRELOAD=./privacy.so cat cannotRemove.txt
ssize_t write(int fildes, const void *buf, size_t nbyte) {
    static __thread int active = 0; // flag so there won't be a loop

    if (fildes == 1 && !active) {
        active = 1;
        printf("Hijacked write() called!\n");
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
        "::::;xxXX$XxXXXXXXXXXXXXXXXXXxxxx++++++xxxXXXXXXXXXXX$$$XXX$XXXXXXx++;;;;;\n"
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
} // die fantastische bilder ka me uf https://www.asciiart.eu/image-to-ascii
