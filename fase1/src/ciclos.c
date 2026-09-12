#include <stdlib.h>
#include "ciclos.h"
#include "log.h"

#define SEM_COR -1

/* ---------- Fila autoral (array circular), mesmo padrão de bfs.c ---------- */

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

/* ---------- Ciclos + bipartição ---------- */

ResultadoCiclosBipartido detectar_ciclos_bipartido(Grafo *g, int *cor) {
    Cronometro c;
    iniciar_cronometro(&c);

    ResultadoCiclosBipartido resultado;
    resultado.tem_ciclo = 0;
    resultado.eh_bipartido = 1;

    for (int i = 0; i < g->num_vertices; i++) {
        cor[i] = SEM_COR;
    }

    int *pai = log_malloc(g->num_vertices * sizeof(int));
    Fila *fila = criar_fila(g->num_vertices);

    /* Cobre o grafo inteiro: repete a partir de cada vértice ainda
     * sem cor, pois o grafo pode ter mais de um componente (#11). */
    for (int origem = 0; origem < g->num_vertices; origem++) {
        if (cor[origem] != SEM_COR) continue;

        cor[origem] = 0;
        pai[origem] = -1;
        enfileirar(fila, origem);

        while (!fila_vazia(fila)) {
            int atual = desenfileirar(fila);
            NoAdjacente *vizinho = g->lista_adj[atual];
            while (vizinho != NULL) {
                int destino = vizinho->destino;

                if (cor[destino] == SEM_COR) {
                    cor[destino] = 1 - cor[atual];
                    pai[destino] = atual;
                    enfileirar(fila, destino);
                } else if (destino != pai[atual]) {
                    /* aresta para vértice já colorido que não é o
                     * pai direto -> aresta de retorno -> tem ciclo */
                    resultado.tem_ciclo = 1;
                    if (cor[destino] == cor[atual]) {
                        resultado.eh_bipartido = 0;
                    }
                }

                vizinho = vizinho->prox;
            }
        }
    }

    destruir_fila(fila);
    log_free(pai);

    double tempo_ms = parar_cronometro(&c);
    registrar_log("CiclosBipartido", g->num_vertices, tempo_ms, memoria_pico_bytes());

    return resultado;
}