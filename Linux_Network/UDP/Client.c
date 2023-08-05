#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int ret = 0;
    char buff[128] = {0,};
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(6000);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1)
    {
        printf("input:\n");
        fgets(buff, 127, stdin);
        if (strncmp(buff, "end", 3) == 0)
        {
            break;
        }

        sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr*)&saddr, sizeof(saddr));
        memset(buff, 0, 128);
        socklen_t len = sizeof(saddr);
        recvfrom(sockfd, buff, sizeof(buff) - 1, 0, (struct sockaddr*)&saddr, &len);
        printf("recv = %s\n", buff);
    }

    ret = close(sockfd);
    assert(ret != -1);
}