#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<string.h>

//Declaracion de variables globales
int **bufferini;
int bufferfin;
int nNumeros;

int i=0,j=0,consum=0;

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
	int num,i,a;
	int esPrimo=1;	
	for(i=0;i<num;i++)
	{
		if(num%i==0)
		a++;		
	}
	if(a==2);
	return esPrimo;
	
}

void productor(int nNum){
	int i;
	for(i=0;i<nNum;i++){
		int numAleat = rand();
		sem_wait(&espacioB1);							
		buffer1[i % tam_buffer] = numAleat;
		sem_post(&datoB1);
	}
		
		









	
