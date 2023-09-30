#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <memory.h>
#include <string.h>

/*

 CAIO CEZAR OLIVEIRA FILARDI DO CARMO - 987307908
 GABRIEL AUGUSTO RIBEIRO GOMES - 32134762
 LUCAS TONETO PIRES - 31817017
 RICARDO GABRIEL MARQUES DOS SANTOS RUIZ - 32134908

*/

/* ---------------------------------------------------------------------------- fim cabecalho | inicio de definicoes */


/*
	Definicoes
*/

 /* Foi utilizado esse valor ao inves de INT_MAX para não ocorrer overflow na hora da soma para comparacao no algoritmo de dijkstra */
#define INFINITO 999999 
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
	int front; /* index do comeco da fila */
	int rear; /* index do fim da fila */
	int size; /* tamanho atual da fila */
	int capacity; /* capacidade total da fila */
	QUEUE_DATA* data; /* array do tipo QUEUE_DATA (nesse caso, vertice) que armazena os dados que manipulamos */
} MinPriorityQueue;
/*
 * Declaracoes das funcoes para manipulacao de filas
 */
MinPriorityQueue* createQueue(int capacity);
/* Verifica se a fila esta cheia */
int isQueueFull(MinPriorityQueue* queue);
/* Insere na fila de prioridade*/
void enqueue(MinPriorityQueue* queue, QUEUE_DATA item);
/* Remove da fila de prioridade*/
QUEUE_DATA dequeue(MinPriorityQueue* queue);
/* Imprime a fila de prioridade */
void imprimeFila(MinPriorityQueue* queue);
/* Desaloca fila de prioridade*/
void destroiFila(MinPriorityQueue* queue);
/* Atualiza um item da fila*/
void updateItem(MinPriorityQueue* queue, int uniqueKey, int distancia, Vert* pai);



/*
 * Declaracoes das funcoes para encontrar o caminho mais curto
 */
 /* Cria o grafo do mapa e atribui na variavel gloabal G*/
void gerarGrafo();
/* Imprime o caminho mais curto de um vertice */
void imprimirCaminho(int origem, int destino, Vert G[]);
/* Executa o algoritmo de dijkstra */
void dijkstra(int origem);

/* Variaveis Globais*/
Vert* G;
int ordemG = 27;
MinPriorityQueue* queue;



/* ---------------------------------------------------------------------------- fim declaracoes | inicio de implementacoes */


/*
	Implementacao Grafo
*/
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
	G[v1].distancia = INFINITO;
	G[v1].prim = A1;

	if (v1 == v2) return 1; /* Aresta e� um laco */

	/* Acrescento aresta na lista do vertice v2 se v2 != v1 */
	A2 = (Aresta*)malloc(sizeof(Aresta));
	A2->valor = valor;
	A2->nome = v1;
	A2->prox = G[v2].prim;
	G[v2].pai = NULL;
	G[v2].distancia = INFINITO;
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
		/* Feito para verificar se o pai eh nulo.
			Caso o pai seja nulo, entao nao imprimimos o valor do mesmo
			Evitando assim segmentation fault */
		if (G[i].pai == NULL)
			printf("\n    -> v%d (pi = NULL, d = %d): ", i, G[i].distancia);
		else
			printf("\n    -> v%d (pi = %d, d = %d): ", i, (G[i].pai)->nome, G[i].distancia);

		for (; aux != NULL; aux = aux->prox)
		{
			printf("  v%d (c=%03d)", aux->nome, aux->valor);
		}
	}
	printf("\n\n  -----------------------------------------------------  \n");
	printf("\n\n");
}

/*
	Implementacao fila de prioridade
*/

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
	/* Se o tamanho eh 0, basta inserir no fim da fila */
	if (queue->size == 0)
	{
		queue->rear = (queue->rear + 1) % queue->capacity;
		queue->data[queue->front] = item;
		queue->size = queue->size + 1;
		return;
	}
	/* Caso o contrario, devemos definir a prioridade*/
	int size = queue->rear + 1;
	for (i; i < size; i++)
	{
		QUEUE_DATA current = queue->data[i];
		if (current == NULL)
			continue; /*Para casos onde deletamos um item*/
		if (item->distancia < current->distancia)
		{
			/* Se encontramos um valor menor, eh pq encontramos a prioridade*/
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
			/* Agora que encontramos a prioridade, podemos finalmente quebrar o laco */
			break;
		}
		/* Se nao encontramos, eh pq a prioridade eh baixa (final da fila) */
		else if (i == queue->rear)
		{
			queue->rear = (queue->rear + 1) % queue->capacity;
			queue->data[i + 1] = item;
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
		QUEUE_DATA elem = queue->data[j];
		if (elem == NULL)
		{
			printf("");
		}
		else
		{
			printf(" %d ", elem->distancia);
		}
		j++;
	}
	printf("\n");
}
void destroiFila(MinPriorityQueue* queue)
{
	free(queue->data);
	free(queue);
}

/* Fim das estruturas de dados */


/* Cria o grafo que iremos utilizar no trabalho, e armazena na variável global G*/
void gerarGrafo()
{
	criaGrafo(&G, ordemG);
	/* v0 */
	acrescentaAresta(G, ordemG, 0, 1, 88);
	/* v1 */
	acrescentaAresta(G, ordemG, 1, 2, 85);
	acrescentaAresta(G, ordemG, 1, 14, 400);
	/* v2 */
	acrescentaAresta(G, ordemG, 2, 7, 220);
	/* v3 */
	acrescentaAresta(G, ordemG, 3, 4, 350);
	/* v4 */
	acrescentaAresta(G, ordemG, 4, 5, 190);
	/* v5 */
	acrescentaAresta(G, ordemG, 5, 6, 210);
	/* v6 */
	acrescentaAresta(G, ordemG, 6, 15, 1000);
	/* v7 */
	acrescentaAresta(G, ordemG, 7, 8, 220);
	/* v8 */
	acrescentaAresta(G, ordemG, 8, 9, 450);
	/* v9 */
	acrescentaAresta(G, ordemG, 9, 10, 400);
	/* v10 */
	acrescentaAresta(G, ordemG, 10, 11, 210);
	/* v11 */
	acrescentaAresta(G, ordemG, 11, 12, 25);
	/* v12 */
	acrescentaAresta(G, ordemG, 12, 13, 325);
	/* v13 */
	acrescentaAresta(G, ordemG, 13, 25, 270);
	/* v14 */
	acrescentaAresta(G, ordemG, 14, 3, 350);
	acrescentaAresta(G, ordemG, 14, 15, 130);
	acrescentaAresta(G, ordemG, 14, 16, 300);
	/* v16 */
	acrescentaAresta(G, ordemG, 16, 17, 130);
	acrescentaAresta(G, ordemG, 16, 21, 180);
	/* v17 */
	acrescentaAresta(G, ordemG, 17, 18, 74);
	acrescentaAresta(G, ordemG, 17, 26, 400);

	/* v18 */
	acrescentaAresta(G, ordemG, 18, 19, 300);
	/* v19 */
	acrescentaAresta(G, ordemG, 19, 20, 225);
	/* v21 */
	acrescentaAresta(G, ordemG, 21, 22, 300);
	/* v22 */
	acrescentaAresta(G, ordemG, 22, 23, 210);
	acrescentaAresta(G, ordemG, 22, 24, 270);
	/* v24 */
	acrescentaAresta(G, ordemG, 24, 26, 1000);
}
/* Função recursiva para imprimir o caminho mínimo a partir da origem para um vértice */
void imprimirCaminho(int origem, int destino, Vert G[])
{
	if (destino == origem)
	{
		printf("v%d", origem);
		return;
	}
	else if (G[destino].pai == NULL)
	{
		printf("Nao existe caminho de %d para %d", origem, destino);
	}
	else
	{
		imprimirCaminho(origem, G[destino].pai->nome, G);
		printf(" -> v%d", destino);
	}
}


void updateItem(MinPriorityQueue* queue, int uniqueKey, int distancia, Vert* pai)
{
	/* Se a fila estiver vazia, nao tem o que atualizar */
	if (isQueueEmpty(queue))
	{
		fprintf(stderr, "Erro ao atualizar item na fila de prioridade: Fila vazia");
		return;
	}

	int i = 0;
	int found = 0;

	/* Agora basta percorrer para atualizar */
	int size = queue->rear + 1;
	for (i; i < size; i++)
	{
		if (queue->data[i] == NULL) continue;
		if (queue->data[i]->nome == uniqueKey)
		{
			/* Item Encontrado*/

			int originalNome = queue->data[i]->nome;
			Aresta* originalPrim = queue->data[i]->prim;

			/* Alocamos um novo vertice com o novo valor 
				Se alterarmos o valor do atual, teriamos que rearranjar a fila*/
			Vert* novoVert = (Vert*)malloc(sizeof(Vert) * 1);
			novoVert->nome = originalNome;
			novoVert->prim = originalPrim;
			novoVert->pai = pai;
			novoVert->distancia = distancia;
			/* Ao inves de rearranjar tudo novamente para remover, vamos apenas atribuir como null ptr*/
			queue->data[i] = NULL;
			/* E entao, enfileirar o novo valor*/
			enqueue(queue, novoVert);
			found = 1;
			break;
		}
	}

	if (!found)
	{
		fprintf(stderr, "Erro ao atualizar item na fila de prioridade: Item com chave unica nao encontrado\n");
		return;
	}
}
/* Preenche a fila com cada vertice de G */
void preencheFila()
{
	int i;
	for (i = 0; i < ordemG; i++)
	{
		enqueue(queue, &(G[i]));
	}
}

/* Algoritmo de Dijkstra*/
void dijkstra(int origem)
{
	/* Inicialização das estruturas de dados - Initialize Single-Source*/
	int i;
	for (i = 0; i < ordemG; i++)
	{
		G[i].distancia = INFINITO;
		G[i].pai = NULL;
	}
	/* Precisamos iniciar a distancia de v0 para v0 como 0, e tambem seu pai como ele mesmo.
		Alem de inserir na fila de prioridade */
	G[origem].distancia = 0;
	G[origem].pai = G;
	preencheFila();


	while (!isQueueEmpty(queue))
	{
		/* Recupera menor valor da fila  */
		QUEUE_DATA u = dequeue(queue);
		/* Se o elemento for nulo, eh pq deletamos ele, então vamos apenas para o proximo */
		/* Fizemos isso para evitar o trabalho de realocar a fila toda vez que um item eh atualizado*/
		if (u == NULL) continue; 
		Aresta* aux;

		/* para cada vertice adjacente a u: */
		aux = u->prim;
		for (; aux != NULL; aux = aux->prox)
		{
			/* Recupera valor de v (vertice adjacente a u)*/
			int v = aux->nome;
			/* Relax(v,u,w(u,v)): passo guloso */
			int dist = G[v].distancia;
			if (dist > u->distancia + aux->valor)
			{
				/* se a distancia nova encontrada eh menor, entao alteramos para essa distancia menor encontrada */
				/* Ajustamos o valor na fila de prioridade, que sera reajustado de acordo com seu valor
						Como eh uma fila de prioridade, os valores menores ficarao no comeco da fila */
				updateItem(queue, G[v].nome, u->distancia + aux->valor, u);
				G[v].distancia = u->distancia + aux->valor;
				G[v].pai = u;
			}
		}
	}

}

int main()
{
	/* Aqui criamos as estruturas que vamos utilizar
		(utilizamos variaveis globais para não precisar passar como referencia e para facilitar as funcoes) */
	queue = createQueue(1000);
	gerarGrafo();

	int origem = 0; /* Define o vértice de origem para o algoritmo de Dijkstra. Nesse caso, o v0*/
	dijkstra(origem); /* Executa o algoritmo de dijkstra */
	/* Agora que o algoritmo foi executado, podemos imprimir os caminhos */
	int i = 0;
	for (i = 0; i < ordemG; i++)
	{
		if (i != origem)
		{
			printf("Distancia de v%d para v%d: %d.\n --> Caminho: \t", origem, i, G[i].distancia);
			imprimirCaminho(origem, i, G);
			printf("\n");
		}
	}
	imprimeGrafo(G, ordemG);

	/* Por fim, liberamos os dados da heap */
	destroiGrafo(&G, ordemG);
	destroiFila(queue);

	return 0;
}