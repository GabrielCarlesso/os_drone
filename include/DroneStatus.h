
#ifndef DRONE_STATUS_H
#define DRONE_STATUS_H


#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 1024

typedef struct {
    int sockfd, client_sockfd;
    pthread_mutex_t mutex_nodo;
}Nodo;


typedef struct {

    int ligado;
    double latitude, longitude, altitude;
    double velocidade;
    double distancia_do_inicio;

    
}DroneStatus;

typedef struct {

    DroneStatus droneStatus;
    pthread_mutex_t mutex_droneStatus;
    Nodo nodo;
    char buffer_comandos[BUFFER_SIZE], buffer_respostas[BUFFER_SIZE];
    pthread_mutex_t mutex_buffer_comandos, mutex_buffer_respostas;
    //int index_buffer;

    pthread_cond_t cond_buffer_comandos, cond_buffer_respostas;


}DroneController;

//Inicializa os valores do controller
void setup_controller(DroneController *controller){
    controller->droneStatus.ligado = 0; 
    controller->droneStatus.latitude = 0; controller->droneStatus.longitude = 0; controller->droneStatus.altitude = 0; 
    controller->droneStatus.velocidade = 0;
    controller->droneStatus.distancia_do_inicio = 0;

    memset(controller->buffer_comandos, 0, sizeof(controller->buffer_comandos));
    memset(controller->buffer_respostas, 0, sizeof(controller->buffer_respostas));

    pthread_mutex_init(&controller->mutex_buffer_comandos, NULL);
    pthread_cond_init(&controller->cond_buffer_comandos,NULL);

    pthread_mutex_init(&controller->mutex_buffer_respostas, NULL);
    pthread_cond_init(&controller->cond_buffer_respostas,NULL);

    pthread_mutex_init(&controller->nodo.mutex_nodo, NULL);

    pthread_mutex_init(&controller->mutex_droneStatus, NULL);

}



void addCommandToBuffer(char buffer[], const char* string) {
    size_t bufferLength = strlen(buffer);
    size_t stringLength = strlen(string);
    size_t total = bufferLength +stringLength;

    // Verificar se há espaço suficiente no buffer
    if (bufferLength + stringLength + 1 >= BUFFER_SIZE) { // +1 para o caractere '+'
        printf("Erro: buffer cheio, não é possível adicionar mais strings.\n");
        return;
    }

    // Adicionar "+" se o buffer já contiver uma string
    if (bufferLength > 0) {
        strcat(buffer, "+");
        bufferLength++;
    }

    // Copiar a string para o buffer a partir da posição atual
    strcat(buffer + bufferLength, string);
}

char* getFirstCommandFromBuffer(char buffer[]) {
    // Encontra o primeiro "+" no buffer
    char* firstSpace = strchr(buffer, '+');
    if (firstSpace == NULL) {
        // Não há espaço em branco, então o buffer contém apenas uma string
        char* removedString = strdup(buffer); // Cria uma cópia da string removida
        buffer[0] = '\0'; // Limpa o buffer
        return removedString;
    }

    // Calcula o tamanho da primeira string
    size_t removedStringLength = firstSpace - buffer;

    // Copia a primeira string removida para uma variável separada
    char* removedString = malloc(removedStringLength + 1); // +1 para o caractere nulo
    strncpy(removedString, buffer, removedStringLength);
    removedString[removedStringLength] = '\0';

    // Remove a primeira string do buffer, movendo o restante para o início
    size_t remainingLength = strlen(firstSpace + 1);
    memmove(buffer, firstSpace + 1, remainingLength + 1);

    return removedString;
}



#endif
