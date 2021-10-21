#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <openssl/conf.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include "base64.c"

#define MAX 200
#define PORT 2555
#define SA struct sockaddr

static const unsigned char iv[] = "01020304050607";
static const unsigned char k_prime[] = "0011223344556677";
char k[MAX];
char key[MAX];

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

void get_key()
{
    unsigned char buff[16];
    RAND_bytes(buff, sizeof(buff));
    // printf("NUmber: %u\n",(int)buff);
    unsigned char buff2[33];
    RAND_bytes(buff2, sizeof(buff2));
    // printf("NUmber: %u\n",(int)buff2);
    unsigned long alt = (unsigned int)buff;
    while (alt > 9999999)
    {
        alt /= 10;
    }
    unsigned long k = (unsigned int)buff2;
    k = k * 1000000 + alt;
    char new[MAX];
    // printf("%lu\n",k);
    // sprintf(new,"%s",aes_enc(k,k_prime));
    printf(new);
    sprintf(key, "%ld", k);
    // printf("%s\n",key);
}

void func(int sock1, int sock2)
{
    char buff[MAX];
    int n;
    get_key();
    char enc_key[200];
    aes_enc(key, k_prime, enc_key);
    printf("%s\n", key);

    read(sock1, buff, sizeof(buff)); //citesc de la A ecb / cbc
    printf("From client: %s\n", buff);
    write(sock1, enc_key, sizeof(enc_key)); //mai trebuie criptata cheia intai si o scriu lui A
    write(sock2, enc_key, sizeof(enc_key));
    write(sock2, buff, sizeof(buff)); //trimit lui B mesajul lui A (ECB / CBC)
    bzero(buff, MAX);
    read(sock2, buff, sizeof(buff));
    printf("%s\n", buff);

    write(sock1, buff, sizeof(buff));

    for (;;)
    {

        bzero(buff, MAX);

        read(sock1, buff, sizeof(buff));
        printf("From client: %s\t", buff);

        write(sock2, buff, sizeof(buff));

        if (strncmp("exit", buff, 4) == 0)
        {
            printf("Server Exit...\n");
            break;
        }
    }
}

int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    int connections[3], number_of_conn = 0;
    while (number_of_conn != 2)
    {
        connfd = accept(sockfd, (SA *)&cli, &len);
        if (connfd < 0)
        {
            printf("server accept failed...\n");
            exit(0);
        }
        else
        {
            printf("server accept the client...%d\n", connfd);
            connections[number_of_conn] = connfd;
            number_of_conn++;
        }
    }
    func(connections[0], connections[1]);

    // char out_new[16][200];
    // char out[200];
    // char out_new[200];
    // char k[] = "123456";

    // aes_enc(k,k_prime,out);
    // aes_dec(out,k_prime,out_new);
    // printf("%s\n",out_new);

    // get_key();
    // strcpy(new,get_key);
    // printf("%s",key);
    // get_key();

    close(sockfd);
}