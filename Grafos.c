#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <memory.h>
#include <string.h>

/*
 CAIO CEZAR OLIVEIRA FILARDI DO CARMO - 
 GABRIEL AUGUSTO RIBEIRO GOMES - 32134762
 LUCAS TONETO PIRES - 31817017
 RICARDO GABRIEL MARQUES DOS SANTOS RUIZ - 32134908

*/


/* 
Definicoes
*/
/*
 * REPRESENTACAO DE GRAFOS - Versao 2023/2
 */

 /*
  * Estrutura de dados para representar grafos
  */

typedef struct aresta Aresta;
typedef struct vert Vert;
typedef struct vert
{  /* Cada vertice tem um ponteiro para uma lista de arestas incidentes nele */
	int nome;
	Aresta* prim;
	Vert* pai;
	int distancia;
}Vert;
typedef struct aresta
{ /* Celula de uma lista de arestas */
	int    nome;
	struct aresta* prox;
	int valor;

}Aresta;
/*
 * Declaracoes das funcoes para manipulacao de grafos
 */
void criaGrafo(Vert** G, int ordem);
void destroiGrafo(Vert** G, int ordem);
int  acrescentaAresta(Vert G[], int ordem, int v1, int v2, int valor);
void imprimeGrafo(Vert G[], int ordem);

/* A fila de prioridade ira armazenar vertices*/
#define QUEUE_DATA Vert*

/*
 * Estrutura de dados para representar filas de prioridade
 */
typedef struct
{
	int front;
	int rear;
	int size;
	int capacity;
	QUEUE_DATA* data;
} MinPriorityQueue;
/*
 * Declaracoes das funcoes para manipulacao de filas
 */
MinPriorityQueue* createQueue(int capacity);
int isQueueFull(MinPriorityQueue* queue);
void enqueue(MinPriorityQueue* queue, QUEUE_DATA item);
QUEUE_DATA dequeue(MinPriorityQueue* queue);
void imprimeFila(MinPriorityQueue* queue);
void destroiFila(MinPriorityQueue* queue);


/*
 * Declaracoes das funcoes para encontrar o caminho mais curto
 */
void gerarGrafo();
void imprimirCaminho(int origem, int destino, Vert G[]);
void dijkstra(int origem);

/* Variaveis Globais*/
Vert* G;
int ordemG = 5;
MinPriorityQueue* queue;


/*
	Implementacao Grafo
*/
#pragma region Graph
/*
 * Criacao de um grafo com ordem predefinida (passada como argumento),
 *   e, inicilamente, sem nenhuma aresta
 */
void criaGrafo(Vert** G, int ordem)
{
	int i;
	*G = (Vert*)malloc(sizeof(Vert) * ordem); /* Alcacao dinamica de um vetor de vertices */

	for (i = 0; i < ordem; i++)
	{
		(*G)[i].nome = i;
		(*G)[i].prim = NULL;    /* Cada vertice sem nenua aresta incidente */
	}
}

/*
 * Deasaloca a memoria dinamica usada para armazenar um grafo.
 */
void destroiGrafo(Vert** G, int ordem)
{
	int i;
	Aresta* a, * n;

	for (i = 0; i < ordem; i++)
	{ /* Remove lista de adjacencia de cada vertice */
		a = (*G)[i].prim;
		while (a != NULL)
		{
			n = a->prox;
			free(a);
			a = n;
		}
	}
	free(*G);
}

/*
 * Acrescenta uma nova aresta em um grafo previamente criado.
 *   Devem ser passados os extremos v1 e v2 da aresta a ser acrescentada
 * Como o grafo nao e orientado, para uma aresta com extremos i e j
 *   serao criadas, na estrutura de dados, arestas (i,j) e (j,i)
 */
int acrescentaAresta(Vert G[], int ordem, int v1, int v2, int valor)
{
	Aresta* A1, * A2;

	if (v1 < 0 || v1 >= ordem) /* Testo se vertices sao validos */
		return 0;
	if (v2 < 0 || v2 >= ordem)
		return 0;

	/* Acrescento aresta na lista do vertice v1 */
	A1 = (Aresta*)malloc(sizeof(Aresta));
	A1->valor = valor;
	A1->nome = v2;
	A1->prox = G[v1].prim;
	G[v1].pai = NULL;
	G[v1].distancia = INT_MAX;
	G[v1].prim = A1;

	if (v1 == v2) return 1; /* Aresta e� um laco */

	/* Acrescento aresta na lista do vertice v2 se v2 != v1 */
	A2 = (Aresta*)malloc(sizeof(Aresta));
	A2->valor = valor;
	A2->nome = v1;
	A2->prox = G[v2].prim;
	G[v2].pai = NULL;
	G[v2].distancia = INT_MAX;
	G[v2].prim = A2;

	return 1;
}


/*
 * Imprime um grafo com uma notacao similar a uma lista de adjacencia.
 */
void imprimeGrafo(Vert G[], int ordem)
{
	printf("\n  -----------------------------------------------------  \n");
	int i;
	Aresta* aux;

	printf("\n\tOrdem:   %d", ordem);
	printf("\n\tc = custo da aresta\n\td = distancia\n\tpi = pai\n");
	printf("\n\tLista de Adjacencia:\n");

	for (i = 0; i < ordem; i++)
	{
		aux = G[i].prim;
		printf("\n    -> v%d (pi = %d, d = %d): ", i, (G[i].pai)->nome, G[i].distancia);
		
		for (; aux != NULL; aux = aux->prox)
		{
			printf("  v%d (c=%003d)", aux->nome, aux->valor);
		}
	}
	printf("\n\n  -----------------------------------------------------  \n");
	printf("\n\n");
}
#pragma endregion

/*
	Implementacao fila de prioridade
*/
#pragma region PriorityQueue



MinPriorityQueue* createQueue(int capacity)
{
	MinPriorityQueue* queue = (MinPriorityQueue*)malloc(sizeof(MinPriorityQueue));
	if (queue == NULL)
	{
		fprintf(stderr, "Erro ao alocar fila de prioridade!\n");
		return queue;
	}
	queue->capacity = capacity;
	queue->front = 0;
	queue->size = 0;
	queue->rear = capacity - 1;
	queue->data = (QUEUE_DATA*)malloc(queue->capacity * sizeof(QUEUE_DATA));

	int i = 0;
	for (i; i < capacity; i++)
	{
		queue->data[i] = NULL;
	}
	return queue;
}
int isQueueFull(MinPriorityQueue* queue)
{
	return (queue->size == queue->capacity);
}
int isQueueEmpty(MinPriorityQueue* queue)
{
	return (queue->size == 0);
}
void enqueue(MinPriorityQueue* queue, QUEUE_DATA item)
{
	if (isQueueFull(queue))
	{
		fprintf(stderr, "Erro ao tentar inserir item na fila de prioridade: Fila cheia");
		return;
	}
	int i = queue->front;
	if (queue->size == 0)
	{
		queue->rear = (queue->rear + 1) % queue->capacity;
		queue->data[queue->front] = item;
		queue->size = queue->size + 1;
		return;
	}
	int size = queue->rear + 1;
	for (i; i < size; i++)
	{
		QUEUE_DATA current = queue->data[i];
		if (item->distancia < current->distancia)
		{
			int j = queue->rear;
			if (j >= queue->capacity)
			{

				fprintf(stderr, "Impossivel adicionar item na fila de prioridade: capacidade insuficiente\n");
				return;
			}
			/* Agora basta mover os demais dados para a direita */
			for (j; j >= i; j--)
			{
				queue->data[j + 1] = queue->data[j];
			}
			/* e entao inserir o valor desejado*/
			queue->rear = (queue->rear + 1) % queue->capacity;
			queue->data[i] = item;
			queue->size = queue->size + 1;
			break;
		}
		else if (i == queue->rear)
		{
			queue->rear = (queue->rear + 1) % queue->capacity;
			queue->data[i+1] = item;
			queue->size = queue->size + 1;
		}
	}

}
QUEUE_DATA dequeue(MinPriorityQueue* queue)
{
	if (isQueueEmpty(queue))
		return NULL;
	QUEUE_DATA item = queue->data[queue->front];
	queue->front = (queue->front + 1) % queue->capacity;
	queue->size = queue->size - 1;
	return item;
}
void imprimeFila(MinPriorityQueue* queue)
{
	printf("Printing queue...\n");
	int j = queue->front;
	int i = 0;
	for (i; i < queue->size; i++)
	{
		printf(" %d ", queue->data[j++]->distancia);
	}
	printf("\n");
}
void destroiFila(MinPriorityQueue* queue)
{
	free(queue->data);
	free(queue);
}

#pragma endregion


/* Cria o grafo que iremos utilizar no trabalho, e armazena na variável global G*/
void gerarGrafo()
{
	criaGrafo(&G, ordemG);
	/* v0 */
	acrescentaAresta(G, ordemG, 0, 1, 10);
	acrescentaAresta(G, ordemG, 0, 4, 5);
	acrescentaAresta(G, ordemG, 0, 3, 7);
	/* v1 */
	acrescentaAresta(G, ordemG, 1, 2, 1);
	acrescentaAresta(G, ordemG, 1, 4, 2);
	acrescentaAresta(G, ordemG, 1, 4, 3);
	/* v2 */
	acrescentaAresta(G, ordemG, 2, 3, 6);
	acrescentaAresta(G, ordemG, 2, 3, 4);
	acrescentaAresta(G, ordemG, 2, 4, 9);
	/* v1 */
	acrescentaAresta(G, ordemG, 3, 4, 2);

}
/* Função para imprimir o caminho mínimo a partir da origem para um vértice */
void imprimirCaminho(int origem, int destino, Vert G[])
{
	if (destino == origem)
	{
		return;
	}
	else if (G[destino].pai == NULL)
	{
		printf("Nao existe caminho de %d para %d", origem, destino);
	}
	else
	{
		imprimirCaminho(origem, G[destino].pai->nome, G);
		printf(" -> v%d", destino, G[destino].distancia);
	}
}
/* Algoritmo de Dijkstra*/ 
void dijkstra(int origem)
{
	/* Inicialização das estruturas de dados - Initialize Single-Source*/ 
	int i;
	for (i = 0; i < ordemG; i++)
	{
		G[i].distancia = INT_MAX;
		G[i].pai = NULL;
	}
	G[origem].distancia = 0;
	G[origem].pai = G;
	enqueue(queue, &(G[origem]));
	while (!isQueueEmpty(queue))
	{
		/* Recupera menor valor da fila  */
		QUEUE_DATA u = dequeue(queue);
		Aresta* aux;
		aux = u->prim;
		/* para cada vertice adjacente a u: */
		for (; aux != NULL; aux = aux->prox)
		{
			/* Recupera valor de v (vertice adjacente a u)*/
			int v = aux->nome;
			/* Relax: passo guloso */
			int dist = G[v].distancia;
			if (dist > u->distancia + aux->valor)
			{
				G[v].distancia = u->distancia + aux->valor;
				G[v].pai = u;
				enqueue(queue, &(G[v]));
			}
		}
	}

	for (i = 0; i < ordemG; i++)
	{
		if (i != origem)
		{
			printf("Distancia de v%d para v%d: %d.\n --> Caminho: \t", origem, i, G[i].distancia);
			imprimirCaminho(origem, i, G);
			printf("\n");
		}
	}
}

int main()
{
	queue = createQueue(1000);
	gerarGrafo();

	int origem = 0; /* Define o vértice de origem para o algoritmo de Dijkstra*/
	dijkstra(origem); /* Executa o algoritmo de dijkstra */

	imprimeGrafo(G, ordemG);


	destroiGrafo(&G, ordemG);
	destroiFila(queue);

	return 0;
}