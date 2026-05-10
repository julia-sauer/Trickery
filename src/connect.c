#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <dlfcn.h>

int config_block_address(const char *ip) {
    FILE *config = fopen("config.txt", "r"); //opens the config file with read permission
    if(config == NULL){
        return 0;
    }
    char line[256];

    while (fgets(line, sizeof(line), config)) {
        if(strncmp(line, "BLOCK_ADDR=", 11) == 0) {
            char *blockedAddr = line + 11;
            blockedAddr[strcspn(blockedAddr, "\n")] = '\0';
            
            printf("Should not connect if ip %s corresponds to some ip in:  %s\n", ip, blockedAddr);        
           
            char *token = strtok(blockedAddr, ",");
            
            while(token != NULL) {

                if(strcmp(ip, token) == 0) {
                    fclose(config);
                    return 1;
                    break;
                }

                token = strtok(NULL,",");
            }
            
        }
    }
    fclose(config);
    return 0;

}

char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
    switch(sa->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
                    s, maxlen);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
                    s, maxlen);
            break;
        
        default:
            strncpy(s, "Unknown AF", maxlen);
            return NULL;
    }
    return s;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    printf("Hijacked function for connect called!\n");
    char ip[INET6_ADDRSTRLEN];
    get_ip_str(addr, ip, sizeof(ip));

    if(config_block_address(ip)) {
        printf("[hook] your are not allowed to connect to %s!\n", ip);
        errno = EACCES;
        return -1;
    }

    int(*real_connect)(int, const struct sockaddr *, socklen_t)=dlsym(RTLD_NEXT, "connect");
    return real_connect(sockfd, addr, addrlen);
}



