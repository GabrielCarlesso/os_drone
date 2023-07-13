#ifndef VERIFICA_POSICAO_H
#define VERIFICA_POSICAO_H


void *verifica_posicao(void *argController) {
    DroneController *controller = (DroneController *)argController; //Estrutura de controle do drone
    struct periodic_info info;      // Estrutura para tornar a thread periodica
    make_periodic (5000000, &info); // Define o periodo de 5s

    while (1)
    {
        
        //printf("\nDistância: ");
        wait_period (&info);
    }
    

}

#endif