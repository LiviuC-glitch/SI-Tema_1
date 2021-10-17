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

char* aes_enc(char plain[MAX],unsigned char key[17])
{
	char new[MAX];
	int len = sizeof(plain);
	AES_KEY enc_key;
	AES_set_encrypt_key(key, sizeof(key), &enc_key);
	AES_encrypt(plain, new, &enc_key);
	printf("Enc key: %s", new);
	return (new);
}

char *aes_dec(char plain[MAX], unsigned char key[17])
{
	char new[MAX];
	int len = sizeof(plain);
	AES_KEY dec_key;
	AES_set_decrypt_key(key, sizeof(key), &dec_key);
	AES_decrypt(plain, new, &dec_key);
	printf("Dec key: %s", new);
	return (new);
}

void ecb_dec();

void func(int sockfd)
{
	char buff[MAX];
	int n;
	bzero(buff, sizeof(buff));
	printf("Enter the string : "); //ECB / CBC
	n = 0;
	while ((buff[n++] = getchar()) != '\n')
		;
	write(sockfd, buff, sizeof(buff));
	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff)); //citesc cheia criptata
	char enc_key[sizeof(buff)];
	strcpy(enc_key, buff); //avem cheia criptata, trebuie decriptata
						   //aici facem decriptarea cheii
	printf("Cripted: %s\n", enc_key);

	//aici va citi de la B:
	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff)); //citesc confirmare de la B
	printf("Confirmare: %s\n", buff);

	for (;;)
	{
		bzero(buff, sizeof(buff));
		printf("Enter the string : "); //incep sa trimit mesaje la B
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(sockfd, buff, sizeof(buff));

		if ((strncmp(buff, "exit", 4)) == 0)
		{
			printf("Client Exit...\n");
			break;
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
