#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <openssl/aes.h>

#define MAX 200
#define PORT 2555
#define SA struct sockaddr

unsigned char iv[] = "01020304050607";
static const unsigned char k_prime[] = "0011223344556677";

int ecb_value = 0;
int cbc_value = 0;
char dec_key[17];

void xor (char str1[], char str2[], char *output[])
{
	int i;
	for (i = 0; i < strlen(str1); i++)
	{
		char temp = str1[i] ^ str2[i];
		output[i] = temp;
	}

	output[i] = '\0';
}

void aes_enc(char plain[200], unsigned char key[16], char *out_new[200])
{
	unsigned char new[10000];
	int len = sizeof(plain);
	AES_KEY enc_key;
	AES_set_encrypt_key((const unsigned char *)key, 128, &enc_key);
	AES_encrypt(plain, out_new, &enc_key);
	// printf("%s",out_new);
}

void aes_dec(char plain[], unsigned char key[16], char *new[])
{
	int len = sizeof(plain);
	AES_KEY dec_key;
	AES_set_decrypt_key((const unsigned char *)key, 128, &dec_key);
	AES_decrypt(plain, new, &dec_key);
	// printf("%s",new);
}

void ecb_dec(char plain[], unsigned char key[16], char *new[])
{
	aes_dec(plain, dec_key, new);
}

void cbc_dec(char plain[], unsigned char key[16], char *new[]){
	char new_text[MAX];
	aes_dec(plain,dec_key,new_text);
	xor(new_text,iv,new);
	sprintf(iv,"%s",plain);
}


void func(int sockfd)
{
	char buff[MAX];
	int n;

	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff)); //citesc cheia
	aes_dec(buff, k_prime, dec_key);
	printf("Decripted: %s\n", dec_key);

	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff)); //citesc ecb / cbc
	printf("From client : %s\n", buff);
	buff[strlen(buff)] = '\0';
	if (strcmp(buff, "ecb\n") == 0)
	{
		ecb_value = 1;
	}
	else if (strcmp(buff, "cbc\n") == 0)
	{
		cbc_value = 1;
	}

	bzero(buff, sizeof(buff));
	sprintf(buff, "ready");
	write(sockfd, buff, sizeof(buff));
	for (int i = 0; i < 4; i++)
	{

		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff)); //citesc de la A

		char decrypted[200];

		if ((strncmp(decrypted, "exit\n", 4)) == 0)
		{
			printf("Client Exit...\n");
			break;
		}
		//voi decoda de la A

		if (ecb_value == 1)
		{
			ecb_dec(buff, dec_key, decrypted);
			printf("%s", decrypted);
		}
		else if (cbc_value == 1)
		{
			printf("Incepem cbc: ");
			cbc_dec(buff,dec_key,decrypted);
			printf("%s\n", decrypted);
		}
	}
	printf("\n");
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
