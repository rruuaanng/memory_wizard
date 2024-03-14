#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "proc_mem_wizard.h"


int main() {
    int fd;
    char version[32] = {0};

    fd = open("/dev/proc_mem_wizard", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    if (ioctl(fd, GET_VERSION, version) < 0) {
        perror("Failed to call ioctl");
        close(fd);
        return 1;
    }
    printf("ioctl: %s\n", version);
    close(fd);
    
    return 0;
}