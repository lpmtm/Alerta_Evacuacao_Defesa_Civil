
#include <stdio.h>
#include <stdlib.h>

#include "grafo.h"

int main(int argc, char *argv[])
{
    const char *caminho = "data/mapa_brumadinho.geojson";

    if (argc > 1) {
        caminho = argv[1];
    }

    Grafo *g = carregar_grafo_geojson(caminho);

    if (!g) {
        fprintf(stderr, "Falha ao carregar o grafo: %s\n", caminho);
        return EXIT_FAILURE;
    }

    imprimir_estatisticas(g);

    int contagem[NUM_TIPOS_VIA] = {0};
    int total = 0;

    for (int i = 0; i < g->num_vertices; i++) {
        for (NoAdjacente *a = g->vertices[i].adjacencias;
             a != NULL;
             a = a->prox) {

            /* Cada aresta não direcionada é contada uma única vez. */
            if (i < a->destino &&
                a->tipo >= 0 &&
                a->tipo < NUM_TIPOS_VIA) {

                contagem[a->tipo]++;
                total++;
            }
        }
    }

    printf("\n%-20s %10s\n", "Tipo da via", "Arestas");
    printf("--------------------------------\n");

    for (int t = 0; t < NUM_TIPOS_VIA; t++) {
        if (contagem[t] > 0) {
            printf("%-20s %10d\n",
                   nome_tipo_via((TipoVia)t),
                   contagem[t]);
        }
    }

    printf("--------------------------------\n");
    printf("TOTAL: %d | esperado: %d | %s\n",
           total,
           g->num_arestas,
           total == g->num_arestas ? "OK" : "ERRO");

    int sucesso = (total == g->num_arestas);

    destruir_grafo(g);

    return sucesso ? EXIT_SUCCESS : EXIT_FAILURE;
}
