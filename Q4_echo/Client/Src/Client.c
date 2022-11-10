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
	char sendbuf[128] = "Client Connetting Successfully";
	char recvbuf[128] = {0,};

	//socket()  connect()  send()  recv()  close()
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert( sockfd != -1);

	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(9876);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //test with ownself

	// try to connect Server
	ret = connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
	assert(ret != -1);

	send(sockfd, sendbuf, strlen(sendbuf), 0);
	memset(sendbuf, 0, sizeof(sendbuf));

	while (1)
	{
		printf("recv:\n");
		if (recv(sockfd, recvbuf, sizeof(recvbuf), 0) > 0)	// --read()
		{
			printf("<<< %s\n", recvbuf);
			memset(recvbuf, 0, sizeof(recvbuf));
		}

		printf("Client Send>>> \n");
		fgets(sendbuf, sizeof(sendbuf), stdin);
		send(sockfd, sendbuf, strlen(sendbuf), 0);  // --write()
		memset(sendbuf, 0, sizeof(sendbuf));
	}

	ret = close(sockfd);
	assert(ret != -1);
}


	




