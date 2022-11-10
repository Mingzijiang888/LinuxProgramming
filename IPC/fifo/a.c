#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

/**
 * #include <sys/types.h>
 * #include <sys/stat.h>
 * int mkfifo(const char *filename, mode_t mode);
 * filename 是管道名 mode 是创建的文件访问权限
 */

int main()
{
    // write only
    int fd = open("FIFO", O_WRONLY);
    assert(fd != -1);

    printf("open FIFO success\n");

    while (1)
    {
        printf("please input << ");
        char buf[128] = {0,};
        fgets(buf, sizeof(buf) - 1, stdin);

        long int writenum = write(fd, buf, strlen(buf) - 1);
        if (writenum != strlen(buf) - 1)
        {
            printf("write error.\n");
        }

        if (strncmp(buf, "end", strlen("end")) == 0)
        {
            break;
        }
    }

    if (close(fd) == -1)
    {
        printf("close failed.");
    }
    exit(0);

}

