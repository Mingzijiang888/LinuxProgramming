#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

int main()
{
    // read only
    int fd = open("FIFO", O_RDONLY);
    assert(fd != -1);

    printf("open FIFO success\n");

    while (1)
    {
        char buf[128] = {0,};
        
        int readnum = read(fd, buf, sizeof(buf) - 1);
        if (readnum <= 0 || strncmp(buf, "end", strlen("end")) == 0)
        {
            break;
        }

        printf("receive >> %s\n", buf);

    }

    if (close(fd) == -1)
    {
        printf("close failed.\n");
    }
    exit(0);

}

