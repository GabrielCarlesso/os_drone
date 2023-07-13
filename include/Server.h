#ifndef SERVER_H
#define SERVER_H


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void *client_commands_reader(void *argController) {
    DroneController *controller = (DroneController *)argController;
    int n;
    char buffer[256];
    
    
    while (1) {
        
        bzero(buffer, sizeof(buffer));
        pthread_mutex_lock(&controller->nodo.mutex_nodo);
        n = read(controller->nodo.client_sockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {// Erro ou cliente fechou a conexão, encerrar thread
            break;
        }
        pthread_mutex_unlock(&controller->nodo.mutex_nodo);

        //Coloca as mensagens recebidas do cliente no buffer de comandos
        pthread_mutex_lock(&controller->mutex_buffer_comandos);
        while((strlen(controller->buffer_comandos) + strlen(buffer)) > (BUFFER_SIZE - 1))
        {
            printf("\nBuffer de comandos cheio\n");
            pthread_cond_wait(&controller->cond_buffer_comandos, &controller->mutex_buffer_comandos);
        }
        addCommandToBuffer(controller->buffer_comandos,buffer);
        //printf("\nBuffer comando: %s\n",controller->buffer_comandos);
        pthread_cond_signal(&controller->cond_buffer_comandos);
        pthread_mutex_unlock(&controller->mutex_buffer_comandos);
        
        
    //Pega a resposta do buffer
    pthread_mutex_lock(&controller->mutex_buffer_respostas);    //Lock para usar o buffer de respsota
    while((strlen(controller->buffer_respostas) == 0))         // Fica travado quando o buffer ta vazio
    {
        //printf("\nBuffer de resposta vazio\n");
        pthread_cond_wait(&controller->cond_buffer_respostas, &controller->mutex_buffer_respostas);
    }

    char* response = getFirstCommandFromBuffer(controller->buffer_respostas);
    printf("\nMensagem retirada do buffer: %s", response);

    pthread_cond_signal(&controller->cond_buffer_respostas);
    pthread_mutex_unlock(&controller->mutex_buffer_respostas);
    pthread_mutex_lock(&controller->nodo.mutex_nodo);
    //Envia a resposta
    n = write(controller->nodo.client_sockfd, response, strlen(response));
    if (n <= 0) {
        printf("Erro ao escrever no socket!\n");
    }
    pthread_mutex_unlock(&controller->nodo.mutex_nodo);
    printf("\nEnviada a Mensagem: %s",response);
        
        
    }
    
    pthread_exit(NULL);
}


void initiateServer(int argc, char *argv[], Nodo *argNodo) {
    
    Nodo *nodo = argNodo;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    int portno;
    
    if (argc < 2) {
        printf("Erro, porta nao definida!\n");
        printf("./SOFTWARE PORTA");
        exit(1);
    }
    nodo->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (nodo->sockfd  < 0) {
        printf("Erro abrindo o socket!\n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(nodo->sockfd , (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("Erro fazendo bind!\n");
        exit(1);
    }
    listen(nodo->sockfd , 1);

    nodo->client_sockfd = accept(nodo->sockfd , (struct sockaddr *) &cli_addr, &clilen);
    if (nodo->client_sockfd < 0) {
        printf("Erro no accept!\n");
        exit(1);
    }

    char *client_ip = inet_ntoa(cli_addr.sin_addr);
    printf("Cliente conectado: %s\n", client_ip);

}



#endif