#include <stdio.h>
#include "grafo.h"

/*
 * Programa de teste da Matriz de Adjacência (issue #4).
 *
 * A estrutura de Matriz de Adjacência já é construída junto com o
 * carregamento do grafo (ver grafo.c / carregar_grafo_geojson).
 * Este programa serve para demonstrar e validar especificamente
 * essa representação.
 *
 * OBS: como o grafo real tem milhares de vértices, imprimimos apenas
 * uma amostra (canto superior esquerdo) da matriz completa, só para
 * fins de demonstração visual.
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

    printf("\n");
    imprimir_matriz_adjacencia(g, 15);

    /* Comparação simples de uso de memória entre as duas representações */
    long memoria_matriz = (long)g->capacidade * g->capacidade * sizeof(int);
    long memoria_lista = (long)g->num_arestas * 2 * sizeof(NoAdjacente);

    printf("\n===== Comparação de memória (estimativa) =====\n");
    printf("Matriz de Adjacência: %ld bytes (~%.2f MB)\n",
           memoria_matriz, memoria_matriz / (1024.0 * 1024.0));
    printf("Lista de Adjacência:  %ld bytes (~%.2f MB)\n",
           memoria_lista, memoria_lista / (1024.0 * 1024.0));
    printf("================================================\n");

    destruir_grafo(g);
    return 0;
}
