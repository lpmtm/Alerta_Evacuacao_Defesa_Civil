#include <stdio.h>
#include "propagacao.h"
#include "log.h"

int simular_propagacao(Grafo *g, int origem, int *camada) {
    if (!g || !camada || origem < 0 || origem >= g->num_vertices) {
        return 0;
    }

    for (int i = 0; i < g->num_vertices; i++) {
        camada[i] = CAMADA_NAO_ATINGIDO;
    }

    int *fila = log_malloc(sizeof(int) * g->num_vertices);
    if (!fila) {
        return 0;
    }

    int inicio = 0;
    int fim = 0;
    fila[fim++] = origem;
    camada[origem] = 0;

    while (inicio < fim) {
        int u = fila[inicio++];
        for (NoAdjacente *a = g->lista_adj[u]; a; a = a->prox) {
            int v = a->destino;
            if (camada[v] == CAMADA_NAO_ATINGIDO) {
                camada[v] = camada[u] + 1;
                fila[fim++] = v;
            }
        }
    }

    log_free(fila);
    return fim;
}

int gravar_afetados_csv(const Grafo *g, const int *camada, const char *caminho) {
    FILE *arquivo = fopen(caminho, "w");
    if (!arquivo) {
        fprintf(stderr, "Aviso: nao foi possivel abrir %s para escrita.\n", caminho);
        return 0;
    }

    fprintf(arquivo, "vertice,lon,lat,camada\n");
    for (int i = 0; i < g->num_vertices; i++) {
        if (camada[i] == CAMADA_NAO_ATINGIDO) continue;
        fprintf(arquivo, "%d,%.7f,%.7f,%d\n", i, g->vertices[i].lon, g->vertices[i].lat, camada[i]);
    }

    fclose(arquivo);
    return 1;
}
