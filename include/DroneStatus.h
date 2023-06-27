
#ifndef DRONE_STATUS_H
#define DRONE_STATUS_H


#include <pthread.h>

#define TRUE 1
#define FALSE 0

typedef struct {
    int sockfd, client_sockfd;
}Nodo;


typedef struct {

    int ligado;
    double latitude, longitude, altitude;
    double velocidade;
    double distancia_do_inicio;

    
}DroneStatus;

typedef struct {
    Nodo nodo;

    char buffer_comandos[1024];
    int index_buffer;
    pthread_mutex_t mutex_buffer_comandos;
    pthread_cond_t buffet_cheio;

}DroneController;

void setup_controller(DroneController *controller){
    pthread_mutex_init(&controller->mutex_buffer_comandos, NULL);
    pthread_cond_init(&controller->buffet_cheio,NULL);
}


#endif