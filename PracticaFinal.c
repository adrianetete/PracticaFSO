#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<string.h>

//Declaracion de variables globales
int nNumeros, tamBuffer1, tamBuffer2;
int *buffer1;
int *buffer2;

int i = 0, j = 0, consum = 0;

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


void *productor(int nNum){
   
    int i;
	int numAleat;
	int indice = nNum;
	
    for( i = 0; i < nNum; i++){
	  
        numAleat = rand() % 100000;
        //sem_wait(&espacioB1);							
        buffer1[indice%tamBuffer1] = numAleat;
		indice++;
        //sem_post(&datoB1);
    }
}

   
int main ( int argc, char* argv[] ) { 

    int j;

    //Comprobar que se introducen solo 3 argumentos
    if (argc != 4){
	   
        fprintf(stderr, "Error en el numero de argumentos\n");
        exit(1);
    }
	
	//Comprobar que el buffer1 es al menos la mitad de nNumeros
	if( atoi(argv[2]) > (atoi(argv[1])/2) ){
		
		fprintf(stderr, "Error. \"Buffer1\" debe ser como maximo la mitad de \"Numeros\".\n");
	}
 
    //Asignacion de las variables
    nNumeros = atoi( argv[1] );
    tamBuffer1 = atoi( argv[2] );
    tamBuffer2 = atoi( argv[3] );

    printf("%d, %d, %d\n", nNumeros, tamBuffer1, tamBuffer2);
    /*
    sem_init(&datoB1, 0, 0);
	
	sem_init(&espacioB1,0, nNumeros);
    */
    //Reserva de memoria para el buffer1
	buffer1 = (int*)malloc(tamBuffer1 * sizeof(int));
    if ( buffer1 == NULL ){
		
        fprintf(stderr, "Error al reservar memoria.\n");
        exit(2);
    }
	
    productor(nNumeros);
	
    for(j = 0; j < tamBuffer1; j ++){
	    
        printf("%d\n", buffer1[j]);
    }
	
    //Fin del programa sin errores
    exit(0);
}
