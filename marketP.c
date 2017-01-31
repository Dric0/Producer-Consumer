#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 1
#define CLIENTES 1
#define GLOBALC 1000

sem_t filas[N];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t produtores[CLIENTES];
pthread_t consumidores[N];

typedef struct Node {
	int valor;
	int tamFila;
	int id;
	struct Node *prox;
}Node;

Node *caixa2[N];

int globalC = 0;
int globalCConsumidos = 0;

int cliente[CLIENTES];

void iCaixa2() {
	int i;
	for (i = 0; i < N; i++) {
		caixa2[i] = malloc(sizeof(Node));
		caixa2[i]->valor = -1;
		caixa2[i]->tamFila = 0;
		caixa2[i]->prox = NULL;
		caixa2[i]->id = -1;
	}
}

int realizarCompra() {
	return globalC + 1;
}

Node *escolherFila2(int id) {
	int ret = 0;
	int menor = caixa2[ret]->tamFila;
	int moeda = 0;
	int i;
	for (i = 0; i < N; i++) {
		if (caixa2[i]->tamFila <= menor) {
			int tmp = ret;
			menor = caixa2[i]->tamFila;
			ret = i;
			if (caixa2[i]->tamFila == caixa2[tmp]->tamFila) {
				moeda = rand() % 100;
				if (moeda > 50) {
					//menor = caixa[i].tamFila;
					//ret = i;
				} else {
					menor = caixa2[tmp]->tamFila;
					ret = tmp;
				}
			}
		}
	}

	Node *novo;
	novo = malloc(sizeof(Node));
	novo->valor = realizarCompra();

	Node *aux;
	Node *aux2;
	aux = caixa2[ret];
	aux2 = caixa2[ret]->prox;
	while (aux->prox != NULL) {
		aux = aux2;
		aux2 = aux2->prox;
	}
	novo->prox = aux2;
	aux->prox = novo;
	novo->id = ret;
	caixa2[ret]->tamFila++;

	return novo;
}

void *fooCliente(void *thread_id) {
	int i = 0;
	int j = 0;
	Node *compra;

	while (globalC != GLOBALC) {
		pthread_mutex_lock(&mutex);
		compra = escolherFila2((int)thread_id);
		globalC++;
		pthread_mutex_unlock(&mutex);
		sem_post(&filas[compra->id]);
		//usleep(0.01);
		j = 0;
		while (j < 0xffffff) j++;
	}
}

int consumirCompra2(int id) {
	int ret = 0;
	Node *lixo;
	lixo = caixa2[id]->prox;
	caixa2[id]->prox = lixo->prox;
	ret = lixo->valor;
	free(lixo);
	caixa2[id]->tamFila--;
	return ret;
}

void *fooCaixa(void *thread_id) {
	int i = 0;
	int j = 0;
	int compra = 0;
	i = (int)thread_id;
	while (globalCConsumidos != GLOBALC) {
		if (caixa2[i]->prox != NULL) {		//FILA NAO VAZIA
			sem_wait(&filas[i]);
			pthread_mutex_lock(&mutex);
			compra = consumirCompra2(i);
			globalCConsumidos++;
			pthread_mutex_unlock(&mutex);
		} else if (caixa2[i]->prox == NULL) {		//FILA VAZIA
			pthread_mutex_lock(&mutex);
			for (j = 0; j < N; j++) {
				if ((caixa2[j]->tamFila > 1) && (j != i)) {
					compra = consumirCompra2(j);
					globalCConsumidos++;
					break;
				}
			}
			pthread_mutex_unlock(&mutex);
		}
		j = 0;
		while (j < 0xffffff) j++;
		//usleep(0.01);
	}
}

int main() {
	iCaixa2();

	srand(time(NULL));
	//iCaixa();
	int i;
	for (i = 0; i < N; i++) {
		sem_init(&filas[i], 0, 0);
	}
	for (i = 0; i < CLIENTES; i++) {
		if (pthread_create(&(produtores[i]), NULL, fooCliente, (void*) i)){
			printf("Erro na criacao da thread.");
		}
	}
	for (i = 0; i < N; i++) {
		if (pthread_create(&(consumidores[i]), NULL, fooCaixa, (void*) i)){
			printf("Erro na criacao da thread.");
		}
	}
	for (i = 0; i < CLIENTES; i++) {
		pthread_join(produtores[i], NULL);
	}
	for (i = 0; i < N; i++) {
		pthread_join(consumidores[i], NULL);
	}

	for (i = 0; i < N; i++) {
		printf("Tamanho da fila do caixa[%i]: %i.\n", i, caixa2[i]->tamFila);
	}
	printf("Numero de compras produzidas: %i.\nNumero de compras consumidas: %i.\n", globalC, globalCConsumidos);

	for (i = 0; i < N; i++) {
		free(caixa2[i]);
	}

	pthread_exit(NULL);
}
