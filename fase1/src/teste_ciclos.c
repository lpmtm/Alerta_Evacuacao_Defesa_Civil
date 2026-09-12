#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "ciclos.h"

/*
 * Programa de teste de ciclos e bipartição (issue #12).
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

    int *cor = malloc(g->num_vertices * sizeof(int));
    ResultadoCiclosBipartido resultado = detectar_ciclos_bipartido(g, cor);

    printf("\nO grafo tem ciclo? %s\n", resultado.tem_ciclo ? "sim" : "nao");
    printf("O grafo e bipartido? %s\n", resultado.eh_bipartido ? "sim" : "nao");

    printf("\nAmostra de cores dos primeiros vertices:\n");
    int limite = g->num_vertices < 15 ? g->num_vertices : 15;
    for (int i = 0; i < limite; i++) {
        printf("  vertice %d: cor=%d\n", i, cor[i]);
    }

    free(cor);
    destruir_grafo(g);
    return 0;
}