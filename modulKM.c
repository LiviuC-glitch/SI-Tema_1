#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <openssl/conf.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

#define MAX 80
#define PORT 2556
#define SA struct sockaddr

char iv[17] = "0102030405060708";
char k_prime[17] = "0011223344556677";
char k[80];
char key[80];

void get_key(){
    unsigned char buff[16];
    RAND_bytes(buff,sizeof(buff));
    // printf("NUmber: %u\n",(int)buff);
    unsigned char buff2[33];
    RAND_bytes(buff2,sizeof(buff2));
    // printf("NUmber: %u\n",(int)buff2);
    unsigned long alt = (unsigned int) buff;
    while (alt>9999999)
    {
        alt/=10;
    }
    unsigned long k = (unsigned int) buff2;
    k = k*1000000 + alt;
    // printf("%lu\n",k);
    sprintf(key,"%ld",k);
    // printf("%s\n",key);
}


void func(int sock1, int sock2)
{
    char buff[MAX];
    int n;
    // k = get_key();

    read(sock1, buff, sizeof(buff));
    printf("From client: %s\t", buff);
    write(sock1, k, sizeof(k));     //mai trebuie criptata cheia intai
    write(sock2,buff,sizeof(buff)); //trimit lui B mesajul lui A (ECB / CBC)

    //MODIFICAT SA TRIMIT LA A SI B ID-URILE FIECARUIA, DUPA CARE A SI B VOR COMUNICA INTRE EI SEPARAT

    for (;;)
    {
        bzero(buff, MAX);

        read(sock1, buff, sizeof(buff));
        printf("From client: %s\t", buff);
        // bzero(buff, MAX);
        // n = 0;
        // while ((buff[n++] = getchar()) != '\n')
        //     ;

        write(sock2, buff, sizeof(buff));

        read(sock2, buff, sizeof(buff));
        printf("From client: %s\t", buff);
        write(sock1, buff, sizeof(buff));


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

    // if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    // {
    //     printf("socket bind failed...\n");
    //     exit(0);
    // }
    // else
    //     printf("Socket successfully binded..\n");

    // if ((listen(sockfd, 5)) != 0)
    // {
    //     printf("Listen failed...\n");
    //     exit(0);
    // }
    // else
    //     printf("Server listening..\n");
    // len = sizeof(cli);

    // int connections[3], number_of_conn = 0;
    // while (number_of_conn != 2)
    // {
    //     connfd = accept(sockfd, (SA *)&cli, &len);
    //     if (connfd < 0)
    //     {
    //         printf("server accept failed...\n");
    //         exit(0);
    //     }
    //     else
    //     {
    //         printf("server accept the client...%d\n", connfd);
    //         connections[number_of_conn] = connfd;
    //         number_of_conn++;
    //     }

        
    // }
    // func(connections[0], connections[1]);

    get_key();
    // strcpy(new,get_key);
    printf("%s",key);
    // get_key();

        close(sockfd);
}