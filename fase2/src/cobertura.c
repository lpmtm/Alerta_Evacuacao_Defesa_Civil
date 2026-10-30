#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cobertura.h"
#include "log.h"

typedef struct {
    int u;
    int v;
} ArestaUnica;


// Backtracking recursivo com poda por limite superior
static void backtracking_cobertura(
    Grafo *g,
    ArestaUnica *lista_arestas,
    int total_arestas,
    int idx_vertice,
    bool *escolhidos,
    int tamanho_atual,
    bool *melhor_escolhidos,
    int *melhor_tamanho
) {
    // Poda: se o tamanho atual já for maior ou igual à melhor solução encontrada, aborta o ramo
    if (tamanho_atual >= *melhor_tamanho) {
        return;
    }

    // Verifica se a seleção atual cobre todas as arestas
    bool todas_cobertas = true;
    for (int i = 0; i < total_arestas; i++) {
        int u = lista_arestas[i].u;
        int v = lista_arestas[i].v;
        if (!escolhidos[u] && !escolhidos[v]) {
            todas_cobertas = false;
            break;
        }
    }

    if (todas_cobertas) {
        if (tamanho_atual < *melhor_tamanho) {
            *melhor_tamanho = tamanho_atual;
            memcpy(melhor_escolhidos, escolhidos, (size_t)g->quantidade_vertices * sizeof(bool));
        }
        return;
    }

    // Caso base de fim de vértices
    if (idx_vertice >= g->quantidade_vertices) {
        return;
    }

    // Decisão 1: Incluir o vertice atual na cobertura
    escolhidos[idx_vertice] = true;
    backtracking_cobertura(
        g, lista_arestas, total_arestas,
        idx_vertice + 1, escolhidos, tamanho_atual + 1,
        melhor_escolhidos, melhor_tamanho
    );

    // Decisão 2: NÃO incluir o vertice atual na cobertura
    escolhidos[idx_vertice] = false;
    backtracking_cobertura(
        g, lista_arestas, total_arestas,
        idx_vertice + 1, escolhidos, tamanho_atual,
        melhor_escolhidos, melhor_tamanho
    );
}

CoberturaVertices resolver_cobertura_exata(Grafo *g) {
    CoberturaVertices res = {NULL, 0};
    if (!g || g->quantidade_vertices <= 0) return res;

    int num_v = g->quantidade_vertices;

    // Extrai lista única de arestas do grafo não-direcionado
    int cap_arestas = g->quantidade_arestas;
    ArestaUnica *lista_arestas = (ArestaUnica *) log_malloc((size_t)cap_arestas * sizeof(ArestaUnica));
    int total_arestas = 0;

    for (int u = 0; u < num_v; u++) {
        for (NoAdjacente *a = g->vertices[u].adjacencias; a != NULL; a = a->prox) {
            if (u < a->destino) { // garante aresta única u < v
                lista_arestas[total_arestas].u = u;
                lista_arestas[total_arestas].v = a->destino;
                total_arestas++;
            }
        }
    }

    bool *escolhidos = (bool *) log_calloc((size_t)num_v, sizeof(bool));
    bool *melhor_escolhidos = (bool *) log_calloc((size_t)num_v, sizeof(bool));
    int melhor_tamanho = num_v + 1; // Pior caso inicial

    backtracking_cobertura(
        g, lista_arestas, total_arestas,
        0, escolhidos, 0,
        melhor_escolhidos, &melhor_tamanho
    );

    log_free(escolhidos);
    log_free(lista_arestas);

    if (melhor_tamanho <= num_v) {
        res.vertices = (int *) log_malloc((size_t)melhor_tamanho * sizeof(int));
        res.tamanho = 0;
        for (int i = 0; i < num_v; i++) {
            if (melhor_escolhidos[i]) {
                res.vertices[res.tamanho++] = i;
            }
        }
    }

    log_free(melhor_escolhidos);
    return res;
}

void liberar_cobertura(CoberturaVertices *cov) {
    if (cov && cov->vertices) {
        log_free(cov->vertices);
        cov->vertices = NULL;
        cov->tamanho = 0;
    }
}