#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <openssl/aes.h>

#define MAX 200
#define PORT 2555
#define SA struct sockaddr

char iv[] = {00, 01, 02, 03, 04, 05, 06, 07};
char k_prime[] = {00, 11, 22, 33, 44, 55, 66, 77};
int ecb_value = 0;
int cbc_value = 0;

void func(int sockfd)
{
	char buff[MAX];
	int n;

	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff)); //citesc ecb / cbc
	printf("From client : %s", buff);
	buff[strlen(buff) - 1] = '\0';
	if (strcmp(buff, "ecb") == 0)
	{
		ecb_value = 1;
	}
	else if (strcmp(buff, "cbc") == 0)
	{
		cbc_value = 1;
	}

	bzero(buff, sizeof(buff));
	sprintf(buff, "ready");
	write(sockfd, buff, sizeof(buff));
	printf("%s\n", buff);

	for (;;)
	{
		printf("Citim acum!\n");

		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff)); //citesc de la A
		buff[strlen(buff) - 1] = '\0';

		// printf("%s\n",buff);

		if ((strncmp(buff, "exit", 4)) == 0)
		{
			printf("Client Exit...\n");
			break;
		}
		//voi decoda de la A

		if (ecb_value == 1)
		{
			printf("Decriptam cu ecb!\n");
			printf("%s\n", buff);
		}
		else if (cbc_value == 1)
		{
			printf("Decriptam cu cbc!\n");
			printf("%s\n", buff);
		}
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and varification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}
