#include <stdio.h>
#include "grafo.h"

/*
 * Programa de teste da Lista de Adjacência (issue #3).
 *
 * A estrutura de Lista de Adjacência já é construída junto com o
 * carregamento do grafo (ver grafo.c / carregar_grafo_geojson).
 * Este programa serve para demonstrar e validar especificamente
 * essa representação.
 */
int main(int argc, char *argv[]) {
    const char *caminho = "../data/mapa_brumadinho.geojson";
    if (argc > 1) {
        caminho = argv[1];
    }

    Grafo *g = carregar_grafo_geojson(caminho);
    if (!g) {
        fprintf(stderr, "Falha ao carregar o grafo.\n");
        return 1;
    }

    imprimir_estatisticas(g);

    printf("\nExibindo apenas os primeiros 15 vértices da Lista de Adjacência ");
    printf("(o grafo completo tem %d vértices):\n\n", g->num_vertices);

    /* Para não poluir o terminal com milhares de linhas, mostramos
     * só uma amostra. Para ver tudo, aumente ou remova o limite abaixo. */
    int limite = g->num_vertices < 15 ? g->num_vertices : 15;
    for (int i = 0; i < limite; i++) {
        printf("%d: ", i);
        NoAdjacente *atual = g->lista_adj[i];
        if (!atual) {
            printf("(sem conexões)");
        }
        while (atual) {
            printf("%d", atual->destino);
            if (atual->prox) printf(" -> ");
            atual = atual->prox;
        }
        printf("\n");
    }

    destruir_grafo(g);
    return 0;
}