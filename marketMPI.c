#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "mpi.h"

//#define RAND_MAX 10

#define N 1
#define CLIENTES 1
#define GLOBALC 100

void produzir(int produtos[]) {
	int i;
	for (i = 0; i <= GLOBALC; i++) {
		produtos[i] = i;
		//printf("%i\n", i);
	}
}

int realizarCompra(int tam, int id, int produtos[]) {
	int i;
	for (i = 0; i < tam; i++) {
		int j = 0;
		while (j < 0xffffff) j++;
		printf("Consumindo compra %i do processo %i.\n", produtos[i], id+1);
	}
}

void fooCaixa(int nProc) {
	int produtos, id, flag = 1;
	//while (1) {
	MPI_Recv(&produtos, GLOBALC/(nProc-1), MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	printf("Pacote de compras recebido pelo processo %i de %i.\n", id+1, nProc);
	realizarCompra(GLOBALC/(nProc-1), id, &produtos);
	MPI_Send(&flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	sleep(2);
	//}
}

void fooCliente(int nProc, int produtor[]) {
	int start = 0, flag = 0, count = 0;
	int i;
	//while (1) {
	for (i = 1; i < nProc; i++) {
		start = (i-1)*(GLOBALC/(nProc-1));
		printf("Enviando pacote de compras para o processo %i de %i.\n\tInicio em pacote[%i]\n", i+1, nProc, start);
		MPI_Send(&produtor[start], GLOBALC/(nProc-1), MPI_INT, i, 0, MPI_COMM_WORLD);
	}
	for (i = 1; i < nProc; i++) {
		MPI_Recv(&flag, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		count++;
		printf("Valor de 'count' apos receive: %i\n", count);
		//if (count == nProc-1) return 0;
	}
	//}
}

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int nProc;
	MPI_Comm_size(MPI_COMM_WORLD, &nProc);

	srand(time(NULL));

	printf("Hello world do processo %d\n", rank+1);
	printf("Numero de processos: %i\n", nProc);

	int produtor[GLOBALC];
	//int pacote = GLOBALC/nProc;

	if (rank == 0) {
		produzir(produtor);
		fooCliente(nProc, produtor);
	} else {
		fooCaixa(nProc);
		//MPI_Finalize();
	}

	/*printf("Chegou aqui\n");
	int finalize_retcode = MPI_Finalize();
    if(0 == rank) fprintf(stderr, "Process, return_code\n");
    fprintf(stderr, "%i, %i\n", rank, finalize_retcode);
	*/

	MPI_Finalize();

	return 0;
}
