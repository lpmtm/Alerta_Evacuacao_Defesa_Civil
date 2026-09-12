#include <stdlib.h>
#include "bfs.h"
#include "log.h"

/* ---------- Fila autoral (array circular) ---------- */

typedef struct {
    int *dados;
    int capacidade;
    int inicio;
    int fim;
    int tamanho;
} Fila;

static Fila *criar_fila(int capacidade) {
    Fila *f = log_malloc(sizeof(Fila));
    f->dados = log_malloc(capacidade * sizeof(int));
    f->capacidade = capacidade;
    f->inicio = 0;
    f->fim = 0;
    f->tamanho = 0;
    return f;
}

static void enfileirar(Fila *f, int valor) {
    f->dados[f->fim] = valor;
    f->fim = (f->fim + 1) % f->capacidade;
    f->tamanho++;
}

static int desenfileirar(Fila *f) {
    int valor = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;
    return valor;
}

static int fila_vazia(Fila *f) {
    return f->tamanho == 0;
}

static void destruir_fila(Fila *f) {
    log_free(f->dados);
    log_free(f);
}

/* ---------- BFS ---------- */

int bfs(Grafo *g, int origem, int *distancia, int *predecessor) {
    if (origem < 0 || origem >= g->num_vertices) return 0;

    Cronometro c;
    iniciar_cronometro(&c);

    int *visitado = log_calloc(g->num_vertices, sizeof(int));
    for (int i = 0; i < g->num_vertices; i++) {
        distancia[i] = BFS_INFINITO;
        predecessor[i] = -1;
    }

    Fila *fila = criar_fila(g->num_vertices);
    visitado[origem] = 1;
    distancia[origem] = 0;
    enfileirar(fila, origem);

    while (!fila_vazia(fila)) {
        int atual = desenfileirar(fila);
        NoAdjacente *vizinho = g->lista_adj[atual];
        while (vizinho != NULL) {
            int destino = vizinho->destino;
            if (!visitado[destino]) {
                visitado[destino] = 1;
                distancia[destino] = distancia[atual] + 1;
                predecessor[destino] = atual;
                enfileirar(fila, destino);
            }
            vizinho = vizinho->prox;
        }
    }

    destruir_fila(fila);
    log_free(visitado);

    double tempo_ms = parar_cronometro(&c);
    registrar_log("BFS", g->num_vertices, tempo_ms, memoria_pico_bytes());

    return 1;
}