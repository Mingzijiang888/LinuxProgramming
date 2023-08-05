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

	//socket()  connect()  send()  recv()  close()
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert( sockfd != -1);

	struct sockaddr_in saddr;  //指定需要连接服务器的IP和端口
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(9876);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//尝试连接服务器端
	ret = connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
	assert(ret != -1);

	while (1)
	{
        printf("input:\n");
        fgets(buff, 127, stdin);
        if (strncmp(buff, "end", 3) == 0)
        {
            break;
        }
		send(sockfd, buff, strlen(buff), 0);
	    memset(buff, 0, sizeof(buff));
        recv(sockfd, buff, 127, 0);
        printf("recv = %s\n", buff);
	}
	ret = close(sockfd);
	assert(ret != -1);
}

