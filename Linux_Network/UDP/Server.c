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

    struct sockaddr_in saddr, caddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(6000);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
    assert(ret != -1);

    while (1)
    {
        socklen_t len = sizeof(caddr);  //unsigned int
        recvfrom(sockfd, buff, 127, 0, (struct sockaddr*)&caddr, &len);
        //recvfrom(sockfd, buff, 1, 0, (struct sockaddr*)&caddr, &len);  //测试字节流的粘包问题
        printf("recv = %s\n", buff);
        sendto(sockfd, "ok", 2, 0, (struct sockaddr*)&caddr, sizeof(caddr));
    }

    ret = close(sockfd);
    assert(ret != -1);
}