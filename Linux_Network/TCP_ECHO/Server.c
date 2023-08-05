#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	int ret = 0;
	char buff[128] = {0,};
	int clifd = 0;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	struct sockaddr_in saddr, caddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(9876);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //环回测试

	ret = bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
	assert(ret != -1);

	listen(sockfd, 5);	//最多存放5个已完成三次握手的客户请求

    while (1)
    {
        //若有客户端请求连接
        unsigned int len = sizeof(saddr);
        printf("accept wait...\n");
        clifd = accept(sockfd, (struct sockaddr*)&caddr, &len);
        if (clifd < 0)
        {
            continue;
        }
        printf("accept client = %d\n", clifd);

        while (1)  //接收客户端发来的信息并echo回去
        {
            int num = recv(clifd, buff, 127, 0);  //与read()同效
            //int num = recv(clifd, buff, 1, 0);  //测试字节流的粘包问题
            if (num <= 0)
            {
                break;
            }
            printf("recv = %s\n", buff);
            send(clifd, buff, strlen(buff), 0);  //与write()同效
            //send(clifd, "ok", 2, 0);  //测试字节流的粘包问题
            memset(buff, 0, sizeof(buff));
        }
        ret = close(clifd);
        assert(ret != -1);
        printf("one client over\n");
    }

	ret = close(sockfd);
	assert(ret != -1);	
}

