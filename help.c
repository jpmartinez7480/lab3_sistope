#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *sumar(void *ptr_arreglo)
{
	int *ptr_arreglo2 = (int *)ptr_arreglo;
	int contElementos=-1;
	int suma=0;
	while(++(contElementos) < 5){
		suma=suma+ptr_arreglo2[contElementos];
		printf("[%i] Hebra:%i\n",contElementos,suma);
	}
return (void *)suma;
}

int main()
{

	int arregloX[]={6,7,8,9,10};
	int arregloY[]={11,12,13,14,15};

	void * retornox;
	void * retornoy; 

	pthread_t hebraX;

	pthread_t hebraY;

	if(pthread_create(&hebraX, NULL, sumar, &arregloX)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;

	}

	if(pthread_create(&hebraY, NULL, sumar, &arregloY)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;

	}

	if(pthread_join(hebraX, &retornox)) { //Espera a la hebra que está allí, en este caso la hebra de x.
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}

	printf("Retorno de x: %d\n",(int)retornox);

	if(pthread_join(hebraY, &retornoy)) { //Espera a la hebra que está allí, en este caso la hebra de x.
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}

	printf("Retorno de y: %d\n",(int)retornoy);

	int rx=(int)retornox;
	int ry=(int)retornoy;
	int resultadoFinal=rx+ry;

	printf("Resultado final: %d\n",resultadoFinal);

	return 0;
}