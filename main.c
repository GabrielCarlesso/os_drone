#include <stdio.h>
#include <stdlib.h>
#include <signal.h> 
#include <pthread.h>

#include "include/DroneStatus.h"
#include "include/Server.h"
#include "include/timer.h"
#include "include/gps.h"




int main(int argc, char *argv[]) {
    
    pthread_t server_thread, gps_thread;
    DroneStatus drone;
    DroneController controller;
    setup_controller(&controller);

    sigset_t alarm_sig;
    setup_alarm_sig(&alarm_sig); //configurar o conjunto de sinais alarm_sig para uso com os timers POSIX.

    initiateServer(argc, argv, &controller.nodo);  

    pthread_create(&server_thread, NULL, client_commands_reader, (void *)&controller);
    pthread_create(&gps_thread, NULL, gps, NULL);
    

    pthread_join(server_thread, NULL);
    close(controller.nodo.client_sockfd);
    close(controller.nodo.client_sockfd);
    
    return 0; 
}
