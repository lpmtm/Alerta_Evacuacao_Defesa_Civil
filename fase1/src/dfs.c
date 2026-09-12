#include <stdlib.h>
#include "dfs.h"
#include "log.h"

/* Estado compartilhado entre as chamadas recursivas.
 * Evita variáveis globais, passando um "contexto" por ponteiro. */
typedef struct {
    int *visitado;
    int *predecessor;
    int *tempo_entrada;
    int *tempo_saida;
    int tempo;
} ContextoDfs;

static void dfs_visita(Grafo *g, int atual, ContextoDfs *ctx) {
    ctx->visitado[atual] = 1;
    ctx->tempo_entrada[atual] = ctx->tempo++;

    NoAdjacente *vizinho = g->lista_adj[atual];
    while (vizinho != NULL) {
        int destino = vizinho->destino;
        if (!ctx->visitado[destino]) {
            ctx->predecessor[destino] = atual;
            dfs_visita(g, destino, ctx);
        }
        vizinho = vizinho->prox;
    }

    ctx->tempo_saida[atual] = ctx->tempo++;
}

int dfs(Grafo *g, int origem, int *predecessor, int *tempo_entrada, int *tempo_saida) {
    if (origem < 0 || origem >= g->num_vertices) return 0;

    Cronometro c;
    iniciar_cronometro(&c);

    ContextoDfs ctx;
    ctx.visitado = log_calloc(g->num_vertices, sizeof(int));
    ctx.predecessor = predecessor;
    ctx.tempo_entrada = tempo_entrada;
    ctx.tempo_saida = tempo_saida;
    ctx.tempo = 0;

    for (int i = 0; i < g->num_vertices; i++) {
        predecessor[i] = -1;
        tempo_entrada[i] = -1;
        tempo_saida[i] = -1;
    }

    dfs_visita(g, origem, &ctx);

    log_free(ctx.visitado);

    double tempo_ms = parar_cronometro(&c);
    registrar_log("DFS", g->num_vertices, tempo_ms, memoria_pico_bytes());

    return 1;
}