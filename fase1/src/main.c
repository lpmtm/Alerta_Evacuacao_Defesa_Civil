#include <stdio.h>
#include "grafo.h"

int main(int argc, char *argv[]) {
    const char *caminho = "../data/mapa_brumadinho.geojson";

    if (argc > 1) {
        caminho = argv[1]; /* permite passar o caminho por linha de comando */
    }

    printf("Carregando grafo a partir de: %s\n\n", caminho);

    Grafo *g = carregar_grafo_geojson(caminho);
    if (!g) {
        fprintf(stderr, "Falha ao carregar o grafo.\n");
        return 1;
    }

    imprimir_estatisticas(g);

    /* Exemplo: mostra os primeiros 5 vértices e seus vizinhos */
    printf("\nExemplo dos primeiros vértices e suas conexões:\n");
    int limite = g->num_vertices < 5 ? g->num_vertices : 5;
    for (int i = 0; i < limite; i++) {
        printf("Vértice %d (lon=%.6f, lat=%.6f) -> vizinhos: ",
               i, g->vertices[i].lon, g->vertices[i].lat);

        NoAdjacente *atual = g->lista_adj[i];
        if (!atual) {
            printf("(nenhum)");
        }
        while (atual) {
            printf("%d ", atual->destino);
            atual = atual->prox;
        }
        printf("\n");
    }

    destruir_grafo(g);
    return 0;
}
