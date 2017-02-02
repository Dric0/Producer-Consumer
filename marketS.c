#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define N 1
#define CLIENTES 1
#define GLOBALC 1000

typedef struct Node {
	int valor;
	int tamFila;
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
	}
}

int realizarCompra() {
	return globalC + 1;//rand() % 1000;
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
	//free(aux);
	//free(aux2);

	caixa2[ret]->tamFila++;

	return novo;
}

void fooCliente() {
	int i = 0;
	int j = 0;
	int escolha = 0;
	Node *compra;
		for (i = 0; i < CLIENTES; i++) {
			compra = escolherFila2(i);
			globalC++;
			//usleep(0.01);
			j = 0;
			//while (j < 0xffffff) j++;
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

void fooCaixa() {
	int i = 0;
	int j = 0;
	int compra = 0;
		for (i; i < N; i++) {
			if (caixa2[i]->prox != NULL) {
				compra = consumirCompra2(i);
				globalCConsumidos++;
			}
			j = 0;
			while (j < 0xffffff) j++;
		}
}

int main() {
	iCaixa2();
	srand(time(NULL));

	while (globalC != GLOBALC) {
		fooCliente();
		if (globalCConsumidos != GLOBALC)
			fooCaixa();
	}	
	printf("------------------------------------------------> Compras produzidas: %i\n", globalC);

	printf("------------------------------------------------> Compras consumidas: %i\n", globalCConsumidos);
	int i;
	for (i = 0; i < N; i++) {
		free(caixa2[i]);
	}
	return 0;
}
