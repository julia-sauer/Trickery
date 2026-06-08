#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <dlfcn.h>

int config_block_address(const char *ip) {
    FILE *(*real_fopen)(const char *restrict, const char *restrict) = dlsym(RTLD_NEXT, "fopen");
    int (*real_fclose)(FILE *) = dlsym(RTLD_NEXT, "fclose");

    FILE *config = real_fopen("config.txt", "r"); //opens the config file with read permission
    if(config == NULL){ //if file couldn't be found 
        return 0; //return 0, 'false' so the address shouldn't be blocked 
    }
    char line[256]; //define array of chars named line with length 256

    while (fgets(line, sizeof(line), config)) { //reads line per line, in every line max 256 chars, in the file config
        if(strncmp(line, "BLOCK_ADDR=", 11) == 0) { //each line is compared with "BLOCK_ADDR=", which are the first 11 chars
            char *blockedAddr = line + 11; //the ip's to block are in the line from the 12th chars to the end
            blockedAddr[strcspn(blockedAddr, "\n")] = '\0'; //makes sure that if there is a line break, the \n is not read, otherwise the comparison would always be false
            //strcspn finds the first \n and replaces it with \0 (end of string)2
            printf("Should not connect if ip %s corresponds to some ip in:  %s\n", ip, blockedAddr);     
           
            char *token = strtok(blockedAddr, ","); //strtok splits the blockedAddr at every ',' so we have separate tokens (separate ip's in this case) 
            
            while(token != NULL) {

                if(strcmp(ip, token) == 0) { //compares the given ip with all token(s), if they are the same:
                    real_fclose(config); //the configuration file is closed
                    return 1; //1 is returned, so true, which causes the connect function to block this address
                }

                token = strtok(NULL,",");
            }
            
        }
    }
    real_fclose(config); //would be reached if there is no BLOCK_ADDR in the config file or if the relevant ip is not to be blocked
    return 0; //address is not blocked because this returns 0 ~ false

}

char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen) //transforms sockaddr into a readable ip address (used for comparing with the ones in the config file)
{ //sockaddr *sa is the socket address that contains the IP + the protocoltype, *s is the char buffer into which the transfomred IP is written, maxlen is the maxium size of the buffer to prevent overflow.
    switch(sa->sa_family) { //sa_family is a field in each sockaddr which declares which protocol was used, so if AF_INET (IPv4) or AF_INET6 (IPv6)
        case AF_INET: //for IPv4
        //inet_ntop = from network to presentation (from binary to readable)
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), //casts sockaddr *sa to sockaddr_in which is IPv4 specific structure, field sin_addr contains actual IPv4, which is then transformed from binary into readable
                    s, maxlen);
            break;

        case AF_INET6: //for IPv6 (functions the same way as IPv4 case)
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
                    s, maxlen);
            break;
        
        default: //if neither IPv4 nor IPv6
            strncpy(s, "Unknown AF", maxlen);
            return NULL; //writes NULL in the buffer -> error indicator
    }
    return s;
}

//function to block some websites displayed with curl (at the moment there are the ips in config.txt for exampl.com)
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    printf("Hijacked function for connect called!\n");
    char ip[INET6_ADDRSTRLEN]; //define array named ip with length of an IPv6 address so IPv4 and IPv6 addresses can be stored in there
    get_ip_str(addr, ip, sizeof(ip)); //transforming the sockaddr to a 'readable' ip address

    if(config_block_address(ip)) { //calling the function to see if the relevant ip should be blocked; if returns 0 this is skiped if return 1:
        printf("[hook] your are not allowed to connect to %s!\n", ip); //outprinted which ip is blocked
        errno = EACCES; //error message from system
        return -1; //access failed
    }

    int(*real_connect)(int, const struct sockaddr *, socklen_t)=dlsym(RTLD_NEXT, "connect"); //dlsym searches the next connect function in load order
    return real_connect(sockfd, addr, addrlen); //gives back the next connect function in load order, so the 'normal' connect, so the website content is displayed in the terminal
}

//ip for other websites wished to be blocked can be found at www.nslookup.io


