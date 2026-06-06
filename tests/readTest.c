#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("important.txt", O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        return 1;
    }
    char buf[200];
    ssize_t n = read(fd, buf, sizeof(buf) -1);

    if (n < 0) {
        perror("read failed");
        close(fd);
        return 1;
    }
    buf[n] = '\0';
    dprintf(2, "READ RETURNED THIS: \n%s\n", buf);

    close(fd);
    return 0;
}
