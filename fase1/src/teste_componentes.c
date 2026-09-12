#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "componentes.h"

/*
 * Programa de teste de componentes conexos (issue #11).
 *
 * Carrega o grafo real, detecta os componentes e lista os bairros
 * (vértices) que ficaram isolados fora do componente principal.
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

    int *componente = malloc(g->num_vertices * sizeof(int));
    int num_componentes = detectar_componentes(g, componente);

    printf("\nNumero de componentes conexos: %d\n", num_componentes);

    int *isolados = malloc(g->num_vertices * sizeof(int));
    int total_isolados = listar_isolados(g, componente, num_componentes, isolados);

    printf("Vertices fora do componente principal: %d\n", total_isolados);
    int limite = total_isolados < 15 ? total_isolados : 15;
    for (int i = 0; i < limite; i++) {
        printf("  vertice isolado: %d (componente %d)\n",
               isolados[i], componente[isolados[i]]);
    }

    free(componente);
    free(isolados);
    destruir_grafo(g);
    return 0;
}