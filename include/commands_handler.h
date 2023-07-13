#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H


#include <string.h>

void bufferiza_resposta(char *resp, DroneController *controller);

void *commands_handler(void *argController) {
    DroneController *controller = (DroneController *)argController;
    char *command;
    
    while (1)
    {   
        char resposta[250];

        pthread_mutex_lock(&controller->mutex_buffer_comandos);
        while((strlen(controller->buffer_comandos) == 0))         // Fica travado quando o buffer ta vazio
        {
            //printf("\nBuffer de comandos vazio\n");
            pthread_cond_wait(&controller->cond_buffer_comandos, &controller->mutex_buffer_comandos);
        }

        command = getFirstCommandFromBuffer(controller->buffer_comandos);
        pthread_cond_signal(&controller->cond_buffer_comandos);
        pthread_mutex_unlock(&controller->mutex_buffer_comandos);

        printf("\nComando no handler: %s\n", command);

        pthread_mutex_lock(&controller->mutex_droneStatus); 
        if(controller->droneStatus.ligado == 1){
            if(strcmp(command,"localizacao")==0){

            }else if(strcmp(command,"desligar")==0){
                controller->droneStatus.ligado = 0;
                strcpy(resposta, "Desligando o drone!");
                bufferiza_resposta(resposta,controller);
            }else if(strcmp(command,"")==0){
            
            }else if(strcmp(command,"")==0){

            }else{
                strcpy(resposta, "Comando inválido!");
                bufferiza_resposta(resposta,controller);
            }

        }else if(strcmp(command,"ligar")==0){
            controller->droneStatus.ligado = 1;
            strcpy(resposta, "Iniciando voo!");
            bufferiza_resposta(resposta,controller);
        }else{
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


#endif