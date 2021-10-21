#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <openssl/aes.h>

#define MAX 200
#define PORT 2555
#define SA struct sockaddr

static const unsigned char iv[] = "01020304050607";
static const unsigned char k_prime[] = "0011223344556677";
char dec_key[17];

void aes_enc(char plain[], unsigned char key[16], char *out_new[])
{
	unsigned char new[MAX];
	int len = sizeof(plain);
	AES_KEY enc_key;
	AES_set_encrypt_key((const unsigned char *)key, 128, &enc_key);
	AES_encrypt(plain, out_new, &enc_key);
	// printf("%s",out_new);
}

void aes_dec(char plain[200], unsigned char key[16], char *new[200])
{
	int len = sizeof(plain);
	AES_KEY dec_key;
	AES_set_decrypt_key((const unsigned char *)key, 128, &dec_key);
	AES_decrypt(plain, new, &dec_key);
	// printf("%s",new);
}

void stringSplit(const char s[], size_t n, int sockfd)
{
	if (n)
	{
		char chunk[n + 1];
		size_t i = 0;
		for (; *s; ++s)
		{
			if (!isspace((unsigned char)*s))
			{
				chunk[i++] = *s;
				if (i == n)
				{
					chunk[i] = '\0';
					i = 0;
					char new_text[MAX];
					aes_enc(chunk, dec_key, new_text);
					printf("%s", new_text);
					write(sockfd, new_text, sizeof(new_text));
				}
			}
		}
		if (i != 0)
		{
			chunk[i] = '\0';
		}
		// printf("Ceva: %s\n", chunk);
	}
}

void ecb_enc(char plain[MAX], unsigned char key[16], int sockfd)
{
	stringSplit(plain, 15, sockfd);
}

void ecb_dec(char plain[], unsigned char key[16])
{
	// stringSplit(plain, 16);
}



void func(int sockfd)
{
	char buff[MAX];
	char crypto[MAX];
	int n;
	bzero(buff, sizeof(buff));
	printf("Enter the string : "); //ECB / CBC
	n = 0;
	while ((buff[n++] = getchar()) != '\n')
		;
	write(sockfd, buff, sizeof(buff));
	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff)); //citesc cheia criptata

	// strcpy(enc_key, buff); //avem cheia criptata, trebuie decriptata
	// 					   //aici facem decriptarea cheii
	// printf("Decripted: %s\n", buff);
	aes_dec(buff, k_prime, dec_key);
	printf("Decripted: %s\n", dec_key);

	//aici va citi de la B:
	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff)); //citesc confirmare de la B
	printf("Confirmare: %s\n", buff);

	bzero(buff, sizeof(buff));
	char *filename = "plain.txt";
	FILE *fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("Error: could not open file %s", filename);
		return 1;
	}
	char text[MAX];
	fscanf(fp, "%s", text);
	printf("Text: %s\n", text);

	ecb_enc(text, dec_key, sockfd);
	// char new_text[MAX];

	// aes_enc(text, dec_key, new_text);
	// write(sockfd, new_text, sizeof(new_text));

	if ((strncmp(buff, "exit", 4)) == 0)
	{
		printf("Client Exit...\n");
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
