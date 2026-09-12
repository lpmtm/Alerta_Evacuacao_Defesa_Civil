#include <stdlib.h>
#include "propagacao.h"
#include "bfs.h"
#include "log.h"

int simular_propagacao(Grafo *g, int origem, double minutos_por_trecho, ChegadaLama *chegada) {
    if (origem < 0 || origem >= g->num_vertices) return 0;

    Cronometro c;
    iniciar_cronometro(&c);

    int *distancia = log_malloc(g->num_vertices * sizeof(int));
    int *predecessor = log_malloc(g->num_vertices * sizeof(int));

    bfs(g, origem, distancia, predecessor);

    for (int i = 0; i < g->num_vertices; i++) {
        chegada[i].distancia_arestas = distancia[i];
        if (distancia[i] == BFS_INFINITO) {
            chegada[i].tempo_estimado_min = -1.0; /* nunca atingido -> fora de risco */
        } else {
            chegada[i].tempo_estimado_min = distancia[i] * minutos_por_trecho;
        }
    }

    log_free(distancia);
    log_free(predecessor);

    double tempo_ms = parar_cronometro(&c);
    registrar_log("Propagacao", g->num_vertices, tempo_ms, memoria_pico_bytes());

    return 1;
}

int listar_atingidos_ate(Grafo *g, ChegadaLama *chegada, double tempo_limite_min, int *atingidos) {
    int total = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        if (chegada[i].tempo_estimado_min >= 0.0 &&
            chegada[i].tempo_estimado_min <= tempo_limite_min) {
            atingidos[total++] = i;
        }
    }
    return total;
}