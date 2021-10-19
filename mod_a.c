#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <openssl/aes.h>

#define MAX 200
#define PORT 2556
#define SA struct sockaddr

static const unsigned char iv[] = "01020304050607";
static const unsigned char k_prime[] = "0011223344556677";
char dec_key[17];

void aes_enc(char plain[200], unsigned char key[16], char *out_new[200])
{
	unsigned char new[10000];
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

void ecb_enc(char plain[200], unsigned char key[16], char *new[200])
{ //la decriptare scot + la final
	char *token;
	token = strtok(plain, "\n");
	while (token != NULL)
	{
		char crp[100];
		aes_enc(token, dec_key, crp);
		strcat(new, crp);
		strcat(token, "+");
		token = strtok(NULL, "\n");
	}
	// for (int i = 0; i < strlen(plain); i++)
	// {
	// 	char nou[100], nou2[100];
	// 	sprintf(nou, "&c", plain[i]);
	// 	aes_enc(nou, dec_key, nou2);
	// 	strcat(new, nou2);
	// 	strcat(new,"+");
	// }
	printf("New: %s\n",new);
}

void ecb_dec(char plain[200], unsigned char key[16], char *new[200])
{
	char *token;
	token = strtok(plain, "+");
	while (token != NULL)
	{
		char crp[100];
		aes_dec(token, dec_key, crp);
		strcat(new, crp);
		token = strtok(NULL, "+");
	}
}

void splitText(char text[200], char *stuff[200])
{
	char block[100];
	int counter = 0;
	int resize = (strlen(text) - strlen(text) % 16) / 16;
	for (int i = 0; i <= resize; i++)
	{
		char newnew[100];
		strncpy(block, text + i * 17, 17);
		strcat(stuff, block);

		if (counter == 0)
		{
			char *stuff_prime = stuff + 6;
			strcpy(stuff, stuff_prime);

			char *blocky = block + 6;
			strcpy(block, blocky);
		}
		counter++;

		strcat(stuff, "\n");
	}
	printf("%s", stuff);
}

void func(int sockfd)
{
	char buff[MAX];
	char crypto[200];
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

	for (;;)
	{
		bzero(buff, sizeof(buff));
		printf("Enter the string : "); //incep sa trimit mesaje la B
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;

		char mesage_cripted[200];
		splitText(buff, mesage_cripted);
		printf("Mesage: %s",mesage_cripted);
		ecb_enc(buff, dec_key, crypto);
		printf("%s\n", crypto);

		write(sockfd, crypto, sizeof(crypto));

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
