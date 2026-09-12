#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "dfs.h"

/*
 * Programa de teste do DFS (issue #10).
 *
 * Carrega o grafo real (mapa_brumadinho.geojson), roda o DFS a partir
 * do vértice 0 e mostra predecessor/timestamps dos primeiros vértices.
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

    int *predecessor = malloc(g->num_vertices * sizeof(int));
    int *tempo_entrada = malloc(g->num_vertices * sizeof(int));
    int *tempo_saida = malloc(g->num_vertices * sizeof(int));

    int origem = 0;
    if (!dfs(g, origem, predecessor, tempo_entrada, tempo_saida)) {
        fprintf(stderr, "DFS falhou: origem invalida.\n");
        free(predecessor);
        free(tempo_entrada);
        free(tempo_saida);
        destruir_grafo(g);
        return 1;
    }

    printf("\nDFS a partir do vertice %d - amostra dos primeiros vertices:\n\n", origem);
    int limite = g->num_vertices < 15 ? g->num_vertices : 15;
    for (int i = 0; i < limite; i++) {
        printf("vertice %d: predecessor=%d entrada=%d saida=%d\n",
               i, predecessor[i], tempo_entrada[i], tempo_saida[i]);
    }

    free(predecessor);
    free(tempo_entrada);
    free(tempo_saida);
    destruir_grafo(g);
    return 0;
}