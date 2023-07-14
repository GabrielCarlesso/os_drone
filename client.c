#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 256

int sockfd;

void *communication(void *arg) {
    char buffer[BUFFER_SIZE];
    int n;

    while (1) {
        // Enviar mensagem
        printf("\nDigite a mensagem: ");
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE-1, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        n = send(sockfd, buffer, strlen(buffer), 0);
        if (n == -1) {
            printf("Erro escrevendo no socket!\n");
            exit(1);
        }

        // Receber resposta
        bzero(buffer, BUFFER_SIZE);
        n = recv(sockfd, buffer, BUFFER_SIZE-1, 0);
        if (n <= 0) {
            printf("Erro lendo do socket!\n");
            exit(1);
        }
        printf("Resposta do servidor: %s\n", buffer);
    }
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;
    struct hostent *server;
    pthread_t thread;

    if (argc < 3) {
        fprintf(stderr, "Uso: %s nomehost porta\n", argv[0]);
        exit(0);
    }
    int portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Erro criando socket!\n");
        return -1;
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        printf("Erro, host não encontrado!\n");
        return -1;
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Erro conectando!\n");
        return -1;
    }

    printf ("_____Menu_____\nlocalization: Retorna a posição atual\ntimeTo lat long: Retorna o tempo até o local\ndistance: Retorna a distãncia do incio\nreturn: Veriica se está proximo\nvelocimetro: Retorna a velocidade\n");
    pthread_create(&thread, NULL, communication, NULL);

    // Aguardar a thread terminar
    pthread_join(thread, NULL);

    close(sockfd);
    return 0;
}
