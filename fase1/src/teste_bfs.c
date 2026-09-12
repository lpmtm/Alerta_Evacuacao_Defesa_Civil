#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "bfs.h"

/*
 * Programa de teste do BFS (issue #9).
 *
 * Carrega o grafo real (mapa_brumadinho.geojson), roda o BFS a partir
 * do vértice 0 e mostra a distância/predecessor dos primeiros vértices,
 * como uma verificação rápida de sanidade.
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

    int *distancia = malloc(g->num_vertices * sizeof(int));
    int *predecessor = malloc(g->num_vertices * sizeof(int));

    int origem = 0;
    if (!bfs(g, origem, distancia, predecessor)) {
        fprintf(stderr, "BFS falhou: origem invalida.\n");
        free(distancia);
        free(predecessor);
        destruir_grafo(g);
        return 1;
    }

    printf("\nBFS a partir do vertice %d - amostra dos primeiros vertices:\n\n", origem);
    int limite = g->num_vertices < 15 ? g->num_vertices : 15;
    for (int i = 0; i < limite; i++) {
        printf("vertice %d: distancia=%d predecessor=%d\n",
               i, distancia[i], predecessor[i]);
    }

    free(distancia);
    free(predecessor);
    destruir_grafo(g);
    return 0;
}