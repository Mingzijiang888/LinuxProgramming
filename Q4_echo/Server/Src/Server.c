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
	char echobuf[128] = {0,};
	char recvbuf[128] = {0,};
	char sendbuf[128] = {0,};

	int clifd = 0;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert( sockfd != -1);

	struct sockaddr_in saddr, caddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(9876);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //环回测试

	ret = bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
	assert(ret != -1);

	listen(sockfd, 5);	//最多存放5个已完成三次握手的客户请求

	//若有客户端请求连接
	unsigned int len = sizeof(saddr);
	clifd = accept(sockfd, (struct sockaddr*)&caddr, &len);
	if (clifd < 0)
	{
		perror("accpet");
	}
	printf("Accept clifd = %d\n", clifd);

	// communication available	
	while (1)
	{
		printf("recv:\n");
		if (recv(clifd, recvbuf, sizeof(recvbuf), 0) > 0)  //接收客户端信息
		{
			printf("<<< %s\n", recvbuf);
			strcpy(echobuf, recvbuf);
			memset(recvbuf, 0, sizeof(recvbuf));
		}

		printf("Server Send>>> \n");
		fgets(sendbuf, sizeof(sendbuf) - 1, stdin);
		send(clifd, sendbuf, strlen(sendbuf), 0);
		memset(sendbuf, 0, sizeof(sendbuf));
	}

	ret = close(sockfd);
	assert(ret != -1);
	ret = close(clifd);
	assert(ret != -1);
}


	
