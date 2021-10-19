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
int ecb_value = 0;
int cbc_value = 0;
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
	// char *token;
	// token = strtok(plain, "\n");
	// while (token != NULL)
	// {
	// 	char crp[100];
	// 	aes_enc(token, dec_key, crp);
	// 	strcat(new, crp);
	// 	strcat(token, "+");
	// 	token = strtok(NULL, "\n");
	// }
	for(int i=0;i<strlen(plain);i++){
		char nou[100], nou2[100];
		sprintf(nou,"&c",plain[i]);
		aes_enc(nou,dec_key,nou2);
		strcat(new,nou2);
	}
}

void ecb_dec(char plain[200], unsigned char key[16], char *new[200])
{
	char *token;
	token = strtok(plain, "+");
	while (token != NULL)
	{
		char crp[100];
		aes_dec(token, dec_key, crp);
		printf("Des: %s\n",crp);
		strcat(new, crp);
		token = strtok(NULL, "+");
	}
	// int space[100];
	// int counter = 0;
	// for(int i=0;i<strlen(plain);i++){
	// 	if(strcmp(plain[i],"+")==0){
	// 		space[counter]=i;
	// 		counter++;
	// 	}
	// }
	// for(int i=0;i<counter;i++){
	// 	char something[50];
	// 	strcpy(something,plain + space[i])
	// }
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
	// printf("%s\n", buff);

	for (;;)
	{
		printf("Citim acum!\n");

		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff)); //citesc de la A

		char decrypted[200];
		// aes_dec(buff,k_prime,decrypted);
		// decrypted[strlen(decrypted) - 1] = '\0';

		// printf("%s\n",buff);

		if ((strncmp(decrypted, "exit\n", 4)) == 0)
		{
			printf("Client Exit...\n");
			break;
		}
		//voi decoda de la A

		if (ecb_value == 1)
		{
			printf("Decriptam cu ecb!\n");
			printf("%s\n\n\n",buff);
			ecb_dec(buff, dec_key, decrypted);
			printf("%s", decrypted);
		}
		else if (cbc_value == 1)
		{
			printf("Decriptam cu cbc!\n");
			printf("%s\n", decrypted);
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
