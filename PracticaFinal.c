#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

#define N_HILOS 4

//Declaracion de variables globales
int nNumeros, tamBuffer1, tamBuffer2;
int *buffer1;
char **buffer2;

//Indices globales de los buffer1 y buffer2
int indiceB1;
int indiceB2;

//Semaforos que indican si esta vacio o hay dato en los buffers
sem_t espacioB1;
sem_t datoB1;
sem_t espacioB2;
sem_t datoB2;

//Semaforos para garantizar la exclusion mutua en las variables globales
sem_t mutexLeer1;
sem_t mutexLeer2;

//Metodo comprobacion de que sea primo
int esPrimo (int num){
   
    int i, a = 0;
  
    for( i = 1; i <= num; i++) {      
        if( num%i == 0 )
        a++;
    }
    if( a == 2 || num <= 1){
        //Si es primo, retorna 1
        return 1;
    }else{
        //Si no es primo retorna 0
        return 0;
    }    
}

void *productor ( void *arg){
   
    //Variables locales del hilo productor
    int i, numAleat;
    srand ((unsigned) time(NULL));
    
    //Bucle que va generando aleatorios y los guarda en el buffer1
    for( i = 0; i < nNumeros; i++){
       
        //La variable toma un valor entre 0 y 99999
        numAleat = rand() % 100000;

        //Esperar a que haya un hueco en el buffer1
        sem_wait(&espacioB1);

        //los indices del buffer1 funcionan de manera circular
        buffer1[i % tamBuffer1] = numAleat;

        //Indicamos que se ha introducido un dato en el buffer
        sem_post(&datoB1);
    }
    pthread_exit(NULL);
}

void *consumidor (void *arg){

    char *mensaje;
    int j, dato;
    int id = *((int *) arg);

    while ( 1 ){

        if (indiceB1 >= nNumeros) {    
            pthread_exit(NULL);
        }

        //Sincronizamos la lectura del indiceB1
        sem_wait(&mutexLeer1);
        j = indiceB1;
        indiceB1++;
        sem_post(&mutexLeer1); 

        //Sincronizamos para que solo lea un indice al mismo tiempo
        sem_wait(&datoB1);
        dato = buffer1[j % tamBuffer1];     
        //Marcamos libre un espacio del buffer1
        sem_post(&espacioB1);        

        //Metemos el mensaje en el buffer2
        sem_wait(&espacioB2);
        if( esPrimo(dato) == 1 ){
            sprintf(buffer2[j % tamBuffer2], "Hilo: %d. Valor producido numero: %d. Cantidad: %d. Es primo.", id, j, dato);
        }else{
            sprintf(buffer2[j % tamBuffer2], "Hilo: %d. Valor producido numero: %d. Cantidad: %d. No es primo.", id, j, dato);
        }
        sem_post(&datoB2);
     }
}

void *consumidorFinal (void *arg){

    char *mensaje;
    int j;

    while ( 1 ){

        //Cerramos el hilo cuando el buffer no tenga mas numeros que procesar
        if (indiceB2 >= nNumeros) {
            pthread_exit(NULL);
        }

        //Sincronizar la lectura del indice del buffer2
        sem_wait(&mutexLeer2);
        j = indiceB2;
        indiceB2++;
        sem_post(&mutexLeer2); 

        //Esperar a que haya un dato en el buffer2
        sem_wait(&datoB2);        
        mensaje = buffer2[j % tamBuffer2];
        //Marcamos libre un espacio del buffer2
        sem_post(&espacioB2);

        //Imprimimos el mensaje
        printf("%s\n", mensaje);
    }
}

//Metodo principal
int main ( int argc, char* argv[] ) { 

    int i, id_hilo[N_HILOS]; 
    int tamMensaje = 67;

    //Comprobar que se introducen solo 3 argumentos
    if (argc != 4){
       
        fprintf(stderr, "Error en el numero de argumentos\n");
        exit(-1);
    }

    //Asignacion de las variables
    nNumeros = atoi( argv[1] );
    tamBuffer1 = atoi( argv[2] );
    tamBuffer2 = atoi( argv[3] );
    
    //Comprobar que el numero de aleatorios generados no es negativo
    if(nNumeros < 0){

        fprintf(stderr, "Error. No se puede generar un numero negativo de numeros aleatorios.\n");
        exit(1);    
    }

    //Comprobar que el buffer1 es al menos la mitad de nNumeros
    if( tamBuffer1 > (nNumeros/2) ){
        
        fprintf(stderr, "Error. \"Buffer1\" debe ser como maximo la mitad de \"Numeros\".\n");
        exit(2);    
    }

    //Comprobar que el buffer2 es al menos la mitad de nNumeros
    if( tamBuffer2 > (nNumeros/2) ){
        
        fprintf(stderr, "Error. \"Buffer2\" debe ser como maximo la mitad de \"Numeros\".\n");
        exit(2);    
    }

    //Comprobar que el tamaño del buffer1 no es negativo
    if(tamBuffer1 <= 0){

        fprintf(stderr, "Error. El tamaño del Buffer1 no puede ser negativo ni nulo.\n");
        exit(3);
    }

    //Comprobar que el tamaño del buffer2 no es negativo
    if(tamBuffer2 <= 0){

        fprintf(stderr, "Error. El tamaño del Buffer2 no puede ser negativo ni nulo.\n");
        exit(4);
    }

    //Establecemos el tamaño de buffer1 para el semaforo
    sem_init(&espacioB1, 0, tamBuffer1);

    //Iniciamos el buffer1 vacio
    sem_init(&datoB1, 0, 0);

    //El semaforo mutexLeer1 solo va a poder ser usado por un hilo a la vez
    sem_init (&mutexLeer1, 0, 1); 
    
    //Reserva de memoria dinamica para el buffer1
    buffer1 = (int*)malloc(tamBuffer1 * sizeof(int));
    if ( buffer1 == NULL ){
        
        fprintf(stderr, "Error al reservar memoria.\n");
        exit(2);
    }

    //Establecemos el tamaño buffer2 para el semaforo 
    sem_init(&espacioB2, 0, tamBuffer2);
    
    //Iniciamos el buffer2 vacio, no hay datos
    sem_init(&datoB2, 0, 0);

    //El semaforo mutexLeer2 solo va a poder ser usado por un hilo a la vez
    sem_init(&mutexLeer2, 0, 1);

    //Reserva de memoria dinamica para el buffer2
    buffer2 = malloc(tamBuffer2 * sizeof(char *));
    if ( buffer2 == NULL){

        fprintf(stderr, "Error al reservar memoria");
        exit(2);
    }
    //Al ser un array de dos dimensiones hay que reservar memoria para cada elemento
    for (i = 0; i < tamBuffer2; i++) {
        buffer2[i] = malloc(tamMensaje * sizeof(char));
        if ( buffer2[i] == NULL){
            fprintf(stderr, "Error al reservar memoria");
            exit(2);
        }
    }

    //Declaramos los hilos: productor, consumidores intermedios y final
    pthread_t hiloProduce, hiloConsume[N_HILOS], hiloConsumeFinal;

    //Creamos el hilo para el productor
    pthread_create(&hiloProduce, NULL, productor, (void *) NULL);

    //Damos un id a los hilos consumidores intermedios
    for(i = 0; i < N_HILOS; i++){
        id_hilo[i] = i+1;
    }

    //Creamos los hilos para los consumidores intermedios
    for(i = 0; i < N_HILOS; i++){
        pthread_create(&hiloConsume[i], NULL, consumidor, (void *) &id_hilo[i]);
    }

    //Creamos el hilo consumidor final
    pthread_create(&hiloConsumeFinal, NULL, consumidorFinal, (void *) NULL);

    //Esperamos a que acabe el proceso productor
    pthread_join(hiloProduce, NULL);

    //Esperamos a que acaben los procesos consumidores intermedios
    for(i = 0; i < N_HILOS; i++){
        pthread_join(hiloConsume[i], NULL);
    }

    //Esperamos a que acabe el proceso consumidor final
    pthread_join(hiloConsumeFinal, NULL);

    //Damos fin a los semaforos usados
    sem_destroy(&espacioB1);
    sem_destroy(&datoB1);
    sem_destroy(&mutexLeer1);
    sem_destroy(&espacioB2);
    sem_destroy(&datoB2);
    sem_destroy(&mutexLeer2);

    //Fin del programa sin errores
    exit(0);
}