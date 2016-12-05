#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

//Declaracion de variables globales
int nNumeros, tamBuffer1, tamBuffer2;
int *buffer1;
int *buffer2;

//int i = 0, j = 0, consum = 0;

//Semaforos que indican si esta vacio o hay dato en los buffers
sem_t espacioB1;
sem_t datoB1;
sem_t espacioB2;
sem_t datoB2;

//Semaforos para garantizar la exclusion mutua en las variables globales

sem_t mutexI;
sem_t mutexJ;
sem_t mutexConsum;

//Metodo comprobacion de que sea primo
int esPrimo (int num){
   
    int i, a;
  
    for( i = 0; i < num; i++) {
	  
        if( num%i == 0 )
        a++;		
    }

    if( a == 2 ) {

        //Si es divisible, retorna 1
        return 1;
    }else {

        //Si no es divisible retorna 0
        return 0;
	}
}

void *productor (){
   
    //Variables locales del hilo productor
	int indice = tamBuffer1;
    int i, numAleat;
	
	//printf("Nums: %d, indice: %d, i: %d \n", nNumeros, indice, i);
	//Bucle que va generando aleatorios y los guarda en el buffer1
    for( i = 0; i < nNumeros; i++){
	   
	    //La variable toma un valor entre 0 y 99999
        numAleat = rand() % 100000;

        //Esperar a que haya un hueco en el buffer1
        sem_wait(&espacioB1);

        //los indices del buffer1 funcionan de manera circular
        buffer1[indice%tamBuffer1] = numAleat;
		indice++;

        //sem_post(&datoB1);
    } 
}


void *consumTest (){

	int j;

	//Testeo de lo que contiene el buffer1
    for(j = 0; j < tamBuffer1; j ++){
	    
        printf("%d\n", buffer1[j]);
    }

}

//Metodo principal
int main ( int argc, char* argv[] ) { 

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
    
    //Reserva de memoria para el buffer1
	buffer1 = (int*)malloc(tamBuffer1 * sizeof(int));
    if ( buffer1 == NULL ){
		
        fprintf(stderr, "Error al reservar memoria.\n");
        exit(2);
    }

	//Declaramos el hilo productor
	pthread_t tid;
	pthread_t tid2;

	//Creamos el hilo para el productor
	pthread_create(&tid, NULL, productor, NULL);

	//Creamos el hilo para el productor
	pthread_create(&tid2, NULL, consumTest, NULL);

    //Esperamos a que acabe el proceso productor
    pthread_join(tid, NULL);
    //Esperamos a que acabe el proceso productor
    pthread_join(tid2, NULL);

    sem_destroy(&espacioB1);

    //Fin del programa sin errores
    exit(0);
}
