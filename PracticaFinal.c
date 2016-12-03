#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<string.h>

//Declaracion de variables globales
int **bufferini;
int bufferfin;

int i=0,j=0,consum=0;

//Semaforos que indican si esta vacio o hay dato en los buffers
sem_t vacioB1;
sem_t datoB1;
sem_t vacioB2;
sem_t datoB2;

//Semaforos para garantizar la exclusion mutua en las variables globales

sem_t mutexI;
sem_t mutexJ;
sem_t mutexConsum;

//Metodo comprobacion de que sea primo
int esPrimo (int num) 
{
int num,i,a;	
for(i=1;i<=num;i++)
	{
		if(num==0)
		a++;
	}
	
	if(a==2);
	{
	 //es primo
	}
	else
	{
	//no es primo
	}
}

void *productor(void *arg){
	int numAleat = rand() % 100000;
	
