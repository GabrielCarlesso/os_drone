#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H

#include <math.h>
#include <string.h>
#include <errno.h>
#undef R
#define R 6371.0 // Raio médio da Terra em quilômetros
#define DISTANCIA_MAX 10

void bufferiza_resposta(char *resp, DroneController *controller);
double haversineDistance(double lat1, double lon1, double lat2, double lon2);

void *commands_handler(void *argController) {
    DroneController *controller = (DroneController *)argController;
    char *fullCommand = NULL; 
    char *command = NULL;
    double coordenadas[3];
    char *endPtr;
    
    while (1)
    {   
        char resposta[250];
        fullCommand = NULL; 
        command = NULL;
        pthread_mutex_lock(&controller->mutex_buffer_comandos);
        while((strlen(controller->buffer_comandos) == 0))         // Fica travado quando o buffer ta vazio
        {
            //printf("\nBuffer de comandos vazio\n");
            pthread_cond_wait(&controller->cond_buffer_comandos, &controller->mutex_buffer_comandos);
        }

        fullCommand = getFirstCommandFromBuffer(controller->buffer_comandos);
        pthread_cond_signal(&controller->cond_buffer_comandos);
        pthread_mutex_unlock(&controller->mutex_buffer_comandos);
        
        command = strtok(fullCommand, " ");
        int comp = strcmp(command,"timeTo");
        printf("\nComando no handler: %s e %i\n", command, comp);

        pthread_mutex_lock(&controller->mutex_droneStatus); 
        if(controller->droneStatus.ligado == 1){

            /* *****Comando timeto***** */
            if(strcmp(command,"timeTo")==0){

                for (size_t i = 0; i < 2; i++)
                {
                    command = strtok(NULL, " ");
                    errno = 0;
                    coordenadas[i] = strtod(command, &endPtr);
                    if (endPtr == command) {
                        printf("\nErro: Nenhum número foi encontrado na string.\n");
                        strcpy(resposta, "\nNenhum valor encontrado em timeTo");
                        bufferiza_resposta(resposta,controller);
                        continue;
                    } else if (*endPtr != '\0') {
                        printf("\nErro: Caractere não convertido encontrado: %c\n", *endPtr);
                        strcpy(resposta, "\nErro nos valores!");
                        bufferiza_resposta(resposta,controller);
                        continue;
                    } else if (errno == ERANGE) {
                        printf("\nErro: Valor fora do intervalo válido.\n");
                        strcpy(resposta, "\nValor fora do intervalo válido.");
                        bufferiza_resposta(resposta,controller);
                        continue;
                    }    
                }

                //Calcula a distãncia
                double distance, time;
                distance = haversineDistance(coordenadas[0],coordenadas[1], controller->droneStatus.latitude, controller->droneStatus.longitude);
                time = distance/6; //Considerando a velocidade do drone de 6 km/h
                sprintf(resposta, "Tempo de voo até o local: %.2f horas!", time);
                bufferiza_resposta(resposta,controller);

            }else if(strcmp(command,"localization")==0){
                sprintf(resposta, "Latitude: %.5f Longitude: %.5f", controller->droneStatus.latitude,controller->droneStatus.longitude);
                bufferiza_resposta(resposta,controller);
            }
            
            else if(strcmp(command,"desligar")==0){
                controller->droneStatus.ligado = 0;
                strcpy(resposta, "Desligando o drone!");
                bufferiza_resposta(resposta,controller);
            }else if(strcmp(command,"Distance")==0){
                if (controller->droneStatus.distancia_do_inicio > DISTANCIA_MAX)
                {
                    strcpy(resposta, "Drone saindo da área de alcance");
                    bufferiza_resposta(resposta,controller);
                }
                else{
                    strcpy(resposta, "Drone Retornando");
                    bufferiza_resposta(resposta,controller);
                }
            }else if(strcmp(command,"velocimetro")==0){
                    sprintf(resposta, "Velocidade: %.2f Km/h", controller->droneStatus.velocidade);
                    bufferiza_resposta(resposta,controller);
            }else{
                strcpy(resposta, "Comando inválido!");
                bufferiza_resposta(resposta,controller);
            }

        }else if(strcmp(command,"ligar")==0){
            printf("\nLigando");
            controller->droneStatus.ligado = 1;
            strcpy(resposta, "Iniciando voo!");
            bufferiza_resposta(resposta,controller);
        }else if(strcmp(command,"desconectar")==0){
            strcpy(resposta, "Desconectando...");
            bufferiza_resposta(resposta,controller);
            pthread_exit(NULL);
        }
        else{
            strcpy(resposta, "O drone está desligado, use o comando 'ligar'!");
            bufferiza_resposta(resposta,controller);
        }
        pthread_mutex_unlock(&controller->mutex_droneStatus);
        
    }
    
}

void bufferiza_resposta(char *resp, DroneController *controller){

        pthread_mutex_lock(&controller->mutex_buffer_respostas);

        while((strlen(controller->buffer_respostas) + strlen(resp)) > (BUFFER_SIZE -1)) // Trava quando o buffer de resposta ta cheio
        {
            printf("\nBuffer de respostas cheio\n");
            pthread_cond_wait(&controller->cond_buffer_respostas, &controller->mutex_buffer_respostas);
        }
        
        addCommandToBuffer(controller->buffer_respostas, resp);
        pthread_cond_signal(&controller->cond_buffer_respostas);
        pthread_mutex_unlock(&controller->mutex_buffer_respostas);

}

double toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}
double haversineDistance(double lat1, double lon1, double lat2, double lon2) {
    double tolerance = 0.000001;
    if (fabs(lat2) < tolerance && fabs(lon2) < tolerance) {
        printf("O valor é próximo de zero.\n");
        return 0.0;
    }

    double dLat = toRadians(lat2 - lat1);
    double dLon = toRadians(lon2 - lon1);

    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(toRadians(lat1)) * cos(toRadians(lat2)) *
               sin(dLon / 2) * sin(dLon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    double distance = R * c;

    return distance;
}

#endif