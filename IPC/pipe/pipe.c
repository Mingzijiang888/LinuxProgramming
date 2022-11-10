#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sched.h>

/*
 int pipe(int fds[2]);
 pipe()成功返回 0，失败返回-1
 fds[0]是管道读端的描述符
 fds[1]是管道写端的描述符
 */

int main()
{
    int fd[2] = {0,};       // fd[0]:read_only  fd[1]:write_only

    int res = pipe(fd);
    assert(res != -1);

    pid_t pid = fork();
    assert(pid != -1);

    if (pid == 0)
    {
        // 只读
        close(fd[1]);
        char buf[128] = {0,};
        read(fd[0], buf, sizeof(buf) - 1);

        printf("child read >> %s\n", buf);
        
        close(fd[0]);
        exit(0);
    }
    else
    {
        // 只写
        close(fd[0]);
        char send_buf[128] = "hello";
        write(fd[1], send_buf, strlen(send_buf));
        close(fd[1]);
        exit(0);
    }

    exit(0);

}

