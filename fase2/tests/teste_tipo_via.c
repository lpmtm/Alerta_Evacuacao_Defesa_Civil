#include <stdio.h>
#include "grafo.h"

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

    int contagem_lista[NUM_TIPOS_VIA] = {0};
    int contagem_matriz[NUM_TIPOS_VIA] = {0};
    int total_lista = 0;
    int total_matriz = 0;
    int assimetricas = 0;

    for (int i = 0; i < g->num_vertices; i++) {
        for (NoAdjacente *a = g->lista_adj[i]; a; a = a->prox) {
            if (i < a->destino) {
                contagem_lista[a->tipo]++;
                total_lista++;
            }
        }
    }

    for (int i = 0; i < g->num_vertices; i++) {
        for (int j = i + 1; j < g->num_vertices; j++) {
            if (g->matriz_adj[i][j] == 1) {
                contagem_matriz[g->matriz_tipo[i][j]]++;
                total_matriz++;
                if (g->matriz_tipo[i][j] != g->matriz_tipo[j][i]) {
                    assimetricas++;
                }
            }
        }
    }

    printf("\n%-15s %10s %10s\n", "Tipo da via", "Lista", "Matriz");
    printf("-----------------------------------------\n");
    int divergente = 0;
    for (int t = 0; t < NUM_TIPOS_VIA; t++) {
        if (contagem_lista[t] == 0 && contagem_matriz[t] == 0) continue;
        printf("%-15s %10d %10d\n", nome_tipo_via((TipoVia)t), contagem_lista[t], contagem_matriz[t]);
        if (contagem_lista[t] != contagem_matriz[t]) divergente = 1;
    }
    printf("-----------------------------------------\n");
    printf("%-15s %10d %10d\n", "TOTAL", total_lista, total_matriz);

    printf("\nTotal bate com o numero de arestas do grafo: %s\n",
           (total_lista == g->num_arestas && total_matriz == g->num_arestas) ? "SIM" : "NAO");
    printf("Lista e Matriz concordam em todos os tipos: %s\n", divergente ? "NAO" : "SIM");
    printf("Arestas com tipo diferente entre i-j e j-i: %d\n", assimetricas);

    destruir_grafo(g);
    return 0;
}
