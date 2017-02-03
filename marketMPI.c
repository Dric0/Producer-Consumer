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
#define GLOBALC 1000

void produzir(int produtos[]) {
	int i;
	for (i = 0; i < GLOBALC; i++) {
		produtos[i] = i;
	}
	printf("------------------------------------------------> Compras produzidas: %i\n", i);
}

int realizarCompra(int index, int tam, int id, int produtos[]) {
	int i;
	for (i = index; i < tam; i++) {
		int j = 0;
		while (j < 0xffffff) j++;
		//printf("Consumindo produto %i do processo %i.\n", produtos[i], id);
	}
}

void fooCliente(int nCaixas, int produtos[]) {
	MPI_Status status;
	int dest, index = 0, indexmsg = 2, arraymsg = 1;
	int chunksize = GLOBALC/nCaixas;
	// -> Enviando a cada Caixa sua parte do array produtos.
	for (dest = 1; dest <= nCaixas; dest++) {
		//printf("Enviando pacote de compras para o processo %i de %i.\n\tInicio em pacote[%i], tamanho: %i.\n", dest, nCaixas, index, chunksize);
		// -> Enviando valor de index para cada Caixa saber onde comecar no array produtos.
		MPI_Send(&index, 1, MPI_INT, dest, indexmsg, MPI_COMM_WORLD);
		// -> Enviando para cada caixa um pacote de dados com os produtos.
		MPI_Send(&produtos[index], chunksize, MPI_FLOAT, dest, arraymsg, MPI_COMM_WORLD);
		index = index + chunksize;
	}
	// -> Esperando receber a respostas dos Caixas.
	int i;
	for (i = 1; i <= nCaixas; i++) {
		int source = i;
		// -> Recebe o valor de index, assim o root sabe qual parte do array produtos foi processada.
		MPI_Recv(&index, 1, MPI_INT, source, indexmsg, MPI_COMM_WORLD, &status);
		// -> Recebe o pacote de dados processado pelo Caixa (source i).
		MPI_Recv(&produtos[index], chunksize, MPI_FLOAT, source, arraymsg, MPI_COMM_WORLD, &status);
	}
	
}

void fooCaixa(int nCaixas, int produtos[]) {
	MPI_Status status;
	int dest, source = 0, index = 0, indexmsg = 2, arraymsg = 1;
	int chunksize = GLOBALC/nCaixas;
	// -> Recebe valor index indicando onde comeca sua parte no array produtos.
	 MPI_Recv(&index, 1, MPI_INT, source, indexmsg, MPI_COMM_WORLD, &status);
	// -> Recebe o pacote de dados do array produtos, para comecar em index.
	MPI_Recv(&produtos[index], chunksize, MPI_FLOAT, source, arraymsg, MPI_COMM_WORLD, &status);

	// -> Realizando compra/processamento do array.
	int id;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	realizarCompra(index, chunksize*id, id, produtos);
	printf("------------------------------------------------> Compras consumidas pelo Caixa %i: %i\n", id, chunksize);

	dest = 0;
	// -> Enviando valor de index para root, assim ele sabe qual parte do array produtos estava sendo processada.
	MPI_Send(&index, 1, MPI_INT, dest, indexmsg, MPI_COMM_WORLD);
	// -> Envia pacote de dados do array produtos de volta, representando o consumo;
	MPI_Send(&produtos[index], chunksize, MPI_FLOAT, source, arraymsg, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	double start = 0, finish = 0;
	start = MPI_Wtime();
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int nProc;
	MPI_Comm_size(MPI_COMM_WORLD, &nProc);

	srand(time(NULL));

	//printf("Hello world do processo %d\n", rank);
	//printf("Numero de processos: %i\n", nProc);

	int produtos[GLOBALC];
	int nCaixas = nProc-1;

	if (rank == 0) {
		produzir(produtos);
		fooCliente(nCaixas, produtos);
	} else
		fooCaixa(nCaixas, produtos);

	finish = MPI_Wtime();
	printf("Time elapsed: %f\n", finish-start);
	MPI_Finalize();

	return 0;
}
