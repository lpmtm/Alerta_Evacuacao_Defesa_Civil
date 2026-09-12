#include <stdlib.h>
#include "componentes.h"
#include "bfs.h"
#include "log.h"

int detectar_componentes(Grafo *g, int *componente) {
    Cronometro c;
    iniciar_cronometro(&c);

    for (int i = 0; i < g->num_vertices; i++) {
        componente[i] = -1;
    }

    int *distancia = log_malloc(g->num_vertices * sizeof(int));
    int *predecessor = log_malloc(g->num_vertices * sizeof(int));

    int num_componentes = 0;
    for (int v = 0; v < g->num_vertices; v++) {
        if (componente[v] != -1) continue; /* já visitado numa rodada anterior */

        /* bfs() sozinho não marca 'componente', então usamos o
         * resultado (distancia != BFS_INFINITO) pra saber quem
         * o BFS desta rodada alcançou. */
        bfs(g, v, distancia, predecessor);
        for (int i = 0; i < g->num_vertices; i++) {
            if (componente[i] == -1 && distancia[i] != BFS_INFINITO) {
                componente[i] = num_componentes;
            }
        }
        num_componentes++;
    }

    log_free(distancia);
    log_free(predecessor);

    double tempo_ms = parar_cronometro(&c);
    registrar_log("Componentes", g->num_vertices, tempo_ms, memoria_pico_bytes());

    return num_componentes;
}

int listar_isolados(Grafo *g, int *componente, int num_componentes, int *isolados) {
    /* Conta o tamanho de cada componente */
    int *tamanho = log_calloc(num_componentes, sizeof(int));
    for (int i = 0; i < g->num_vertices; i++) {
        tamanho[componente[i]]++;
    }

    /* Acha o maior componente (o "componente principal") */
    int principal = 0;
    for (int i = 1; i < num_componentes; i++) {
        if (tamanho[i] > tamanho[principal]) principal = i;
    }

    int total_isolados = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        if (componente[i] != principal) {
            isolados[total_isolados++] = i;
        }
    }

    log_free(tamanho);
    return total_isolados;
}