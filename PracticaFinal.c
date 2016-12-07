#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

//Declaracion de variables globales
int nNumeros, tamBuffer1, tamBuffer2;
int *buffer1;
int *buffer2;

#define N_HILOS 4

//Indices globales de los buffer1 y buffer2
int indiceB1;

//Semaforos que indican si esta vacio o hay dato en los buffers
sem_t espacioB1;
sem_t datoB1;

//Semaforos para garantizar la exclusion mutua en las variables globales
sem_t mutexLeer;

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
	
    //printf("Nums: %d, indice: %d, i: %d \n", nNumeros, indice, i);
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

	int j, dato;
	int id = *((int *) arg);

	for( j = 0; j < nNumeros; j++ ){

        sem_wait(&datoB1);

        //Sincronizamos para que solo lea un indice al mismo tiempo
        sem_wait(&mutexLeer);
        dato = buffer1[indiceB1 % tamBuffer1];
        indiceB1++;
        sem_post(&mutexLeer);

        if( esPrimo(dato) == 1 ){

            printf("%d es primo. Hilo %d.\n", dato, id);
        }else{

            printf("%d no es primo. Hilo %d.\n", dato, id);
        }
		
        //Marcamos libre un espacio del buffer1
        sem_post(&espacioB1);
    }
    pthread_exit(NULL);
}

//Metodo principal
int main ( int argc, char* argv[] ) { 

    int i; 

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

    printf("%d, %d, %d\n", nNumeros, tamBuffer1, tamBuffer2);
    /*
    sem_init(&datoB1, 0, 0);
    */

    //Establecemos el tamaño de buffer1 para el semaforo
    sem_init(&espacioB1, 0, tamBuffer1);

    //Iniciamos el buffer1 vacio
    sem_init(&datoB1, 0, 0);

    sem_init (&mutexLeer, 0, 1); 
    
    //Reserva de memoria para el buffer1
    buffer1 = (int*)malloc(tamBuffer1 * sizeof(int));
    if ( buffer1 == NULL ){
		
        fprintf(stderr, "Error al reservar memoria.\n");
        exit(2);
    }

    //Declaramos el hilo productor
    pthread_t hiloProduce;

    //Declaramos los hilos consumidores
    pthread_t hiloConsume[N_HILOS];

    //Creamos el hilo para el productor
    pthread_create(&hiloProduce, NULL, productor, (void *) NULL);

    //Creamos los hilos para los consumidores
    for(i = 0; i < N_HILOS; i++){
        pthread_create(&hiloConsume[i], NULL, consumidor, (void *) &i);
    }

    //Esperamos a que acabe el proceso productor
    pthread_join(hiloProduce, NULL);

    //Esperamos a que acabe el proceso consumidor
    for(i = 0; i < N_HILOS; i++){
    	pthread_join(hiloConsume[i], NULL);
    }

    sem_destroy(&espacioB1);
    sem_destroy(&datoB1);
    sem_destroy(&mutexLeer);

    //Fin del programa sin errores
    exit(0);
}
