#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <memory.h>
#include <string.h>




/*
	Graph implementation
*/
#pragma region Graph

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
}Vert;
typedef struct aresta
{ /* Celula de uma lista de arestas */
	int    nome;
	struct aresta* prox;
	int valor;
	Vert* father;
	int distancia;
}Aresta;
/*
 * Declaracoes das funcoes para manipulacao de grafos
 */
void criaGrafo(Vert** G, int ordem);
void destroiGrafo(Vert** G, int ordem);
int  acrescentaAresta(Vert G[], int ordem, int v1, int v2, int valor);
void imprimeGrafo(Vert G[], int ordem);


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
	A1->father = NULL;
	A1->distancia = INT_MAX;
	G[v1].prim = A1;

	if (v1 == v2) return 1; /* Aresta e� um laco */

	/* Acrescento aresta na lista do vertice v2 se v2 != v1 */
	A2 = (Aresta*)malloc(sizeof(Aresta));
	A2->valor = valor;
	A2->nome = v1;
	A2->prox = G[v2].prim;
	A2->father = NULL;
	A2->distancia = INT_MAX;
	G[v2].prim = A2;

	return 1;
}


/*
 * Imprime um grafo com uma notacao similar a uma lista de adjacencia.
 */
void imprimeGrafo(Vert G[], int ordem)
{
	int i;
	Aresta* aux;

	printf("\nOrdem:   %d", ordem);
	printf("\nLista de Adjacencia:\n");
	printf("\nc = custo da aresta\nd = distancia\npi = pai\n");
	for (i = 0; i < ordem; i++)
	{
		printf("\n    v%d: ", i);
		aux = G[i].prim;
		for (; aux != NULL; aux = aux->prox)
		{
			char dist[255];
			if (aux->distancia == INT_MAX)
				strcpy(dist, "INFINITO");
			else
				sprintf(dist, "%d", aux->distancia);
			printf("  v%d (c=%003d:d=%s:pi=%003d) ", aux->nome, aux->valor, dist, aux->father);
		}
	}
	printf("\n\n");
}
#pragma endregion

/*
	Priority Queue implementation
*/
#pragma region PriorityQueue
#define QUEUE_DATA int



typedef struct
{
	int front;
	int rear;
	int size;
	int capacity;
	QUEUE_DATA* data;
} MinPriorityQueue;

MinPriorityQueue* CreateQueue(int capacity);
int isQueueFull(MinPriorityQueue* queue);
void Enqueue(MinPriorityQueue* queue, QUEUE_DATA item);
int Dequeue(MinPriorityQueue* queue);
void PrintQueueData(MinPriorityQueue* queue);
void FreeQueue(MinPriorityQueue* queue);

MinPriorityQueue* CreateQueue(int capacity)
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
		queue->data[i] = INT_MAX;
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
void Enqueue(MinPriorityQueue* queue, QUEUE_DATA item)
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
		if (item < current)
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
int Dequeue(MinPriorityQueue* queue)
{
	if (isQueueEmpty(queue))
		return INT_MIN;
	int item = queue->data[queue->front];
	queue->front = (queue->front + 1) % queue->capacity;
	queue->size = queue->size - 1;
	return item;
}
void PrintQueueData(MinPriorityQueue* queue)
{
	int j = queue->front;
	int i = 0;
	for (i; i < queue->size; i++)
	{
		printf(" %d ", queue->data[j++]);
	}
	printf("\n");
}
void FreeQueue(MinPriorityQueue* queue)
{
	free(queue->data);
	free(queue);
}

#pragma endregion

void TestQueue()
{
	MinPriorityQueue* queue = CreateQueue(300);

	Enqueue(queue, -200);
	PrintQueueData(queue);
	Enqueue(queue, -1);
	PrintQueueData(queue);
	Enqueue(queue, 0);
	PrintQueueData(queue);
	Enqueue(queue, 40);
	PrintQueueData(queue);
	Enqueue(queue, 30);
	PrintQueueData(queue);
	Enqueue(queue, 20);
	PrintQueueData(queue);
	Enqueue(queue, 10);
	PrintQueueData(queue);
	Enqueue(queue, 50);
	PrintQueueData(queue);
	Enqueue(queue, 1);
	PrintQueueData(queue);
	Enqueue(queue, -300);
	PrintQueueData(queue);


	while (!isQueueEmpty(queue))
	{
		QUEUE_DATA value = Dequeue(queue);
		printf("%d dequeued from queue\n\n",
			value);
	}

	printf("Starting enqueue again\n");
	Enqueue(queue, -200);
	PrintQueueData(queue);
	Enqueue(queue, -1);
	PrintQueueData(queue);
	Enqueue(queue, 0);
	PrintQueueData(queue);
	Enqueue(queue, 40);
	PrintQueueData(queue);
	Enqueue(queue, 30);
	PrintQueueData(queue);
	Enqueue(queue, 20);
	PrintQueueData(queue);
	Enqueue(queue, 10);
	PrintQueueData(queue);
	Enqueue(queue, 50);
	PrintQueueData(queue);
	Enqueue(queue, 1);
	PrintQueueData(queue);
	Enqueue(queue, -300);
	PrintQueueData(queue);


	while (!isQueueEmpty(queue))
	{
		QUEUE_DATA value = Dequeue(queue);
		printf("%d dequeued from queue\n\n",
			value);
	}
	PrintQueueData(queue);

	while (!isQueueEmpty(queue))
	{
		QUEUE_DATA value = Dequeue(queue);
		printf("%d dequeued from queue\n\n",
			value);
	}

	printf("Starting enqueue again\n");
	Enqueue(queue, -200);
	PrintQueueData(queue);
	Enqueue(queue, -1);
	PrintQueueData(queue);
	Enqueue(queue, 0);
	PrintQueueData(queue);
	Enqueue(queue, 40);
	PrintQueueData(queue);
	Enqueue(queue, 30);
	PrintQueueData(queue);
	Enqueue(queue, 20);
	PrintQueueData(queue);
	Enqueue(queue, 10);
	PrintQueueData(queue);
	Enqueue(queue, 50);
	PrintQueueData(queue);
	Enqueue(queue, 1);
	PrintQueueData(queue);
	Enqueue(queue, -300);
	PrintQueueData(queue);


	while (!isQueueEmpty(queue))
	{
		QUEUE_DATA value = Dequeue(queue);
		printf("%d dequeued from queue\n\n",
			value);
	}
	PrintQueueData(queue);
	FreeQueue(queue);
}


#pragma region Algo
Vert* G;
int ordemG = 5;
/* Cria o grafo que iremos utilizar no trabalho, e armazena na variável global G*/
void GenerateGraph()
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
#pragma endregion

int main()
{
	/* TestQueue(); */

	GenerateGraph();

	

	imprimeGrafo(G, ordemG);

	destroiGrafo(&G, ordemG);
	return 0;
}