#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H


#include <string.h>

void *commands_handler(void *argController) {
    DroneController *controller = (DroneController *)argController;
    char *command;
    while (1)
    {
        pthread_mutex_lock(&controller->mutex_buffer_comandos);
        while((strlen(controller->buffer_comandos) == 0))
        {
            //printf("\nBuffer de comandos vazio\n");
            pthread_cond_wait(&controller->buffer_not_empty, &controller->mutex_buffer_comandos);
        }

        command = getFirstCommandFromBuffer(controller->buffer_comandos);
        pthread_cond_signal(&controller->buffer_not_full);
        pthread_mutex_unlock(&controller->mutex_buffer_comandos);
        //printf("\nComando no handler: %s\n", command);

        if(strcmp(command,"localizacao")==0){

        }
        else if(strcmp(buffer,"Sair")==0){
            break;
        }
    }
    
}


#endif