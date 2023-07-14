#ifndef VERIFICA_STATUS_H
#define VERIFICA_STATUS_H

#include "DroneStatus.h"
#include <math.h>
#include "commands_handler.h"

void *verifica_status(void *argController) {
    
    DroneController *controller = (DroneController *)argController;
    struct periodic_info info;      // Estrutura para tornar a thread periodica
    make_periodic (60000000, &info); // Define o periodo de 1min
    double coordenadas[3]={0.0};  // 0=lat, 1= long, 2=alt
    double tolerance = 0.0001;

    while (1)
    {
        
        pthread_mutex_lock(&controller->mutex_droneStatus);
        //verica se os valores estão diferente
        if(fabs(controller->droneStatus.latitude- coordenadas[0]) > tolerance && fabs(controller->droneStatus.longitude - coordenadas[1]) > tolerance && fabs(controller->droneStatus.longitude - coordenadas[2]) > tolerance){
            
            double distancia = haversineDistance(coordenadas[0],coordenadas[1], controller->droneStatus.latitude,controller->droneStatus.longitude);
            controller->droneStatus.velocidade = distancia / 0.016666667;
            
            coordenadas[0] = controller->droneStatus.latitude;
            coordenadas[1] = controller->droneStatus.longitude; 
            coordenadas[2] = controller->droneStatus.altitude; 
        }
        pthread_mutex_unlock(&controller->mutex_droneStatus); 


       
        wait_period (&info);
    }
    

}

#endif