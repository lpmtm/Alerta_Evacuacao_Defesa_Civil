#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "propagacao.h"

/*
 * Programa de teste da simulação de propagação (issue #13).
 *
 * NOTA: origem=0 e minutos_por_trecho=2.0 sao valores de exemplo
 * (placeholder), ate que o ponto real de ruptura da barragem e o
 * tempo medio por trecho sejam definidos com o restante da equipe.
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

    int origem = 0;
    double minutos_por_trecho = 2.0;

    ChegadaLama *chegada = malloc(g->num_vertices * sizeof(ChegadaLama));
    if (!simular_propagacao(g, origem, minutos_por_trecho, chegada)) {
        fprintf(stderr, "Simulacao falhou: origem invalida.\n");
        free(chegada);
        destruir_grafo(g);
        return 1;
    }

    printf("\nSimulacao de propagacao a partir do vertice %d (%.1f min/trecho):\n\n",
           origem, minutos_por_trecho);
    int limite = g->num_vertices < 15 ? g->num_vertices : 15;
    for (int i = 0; i < limite; i++) {
        if (chegada[i].tempo_estimado_min < 0) {
            printf("  vertice %d: fora de risco (nao atingido)\n", i);
        } else {
            printf("  vertice %d: %d trechos, chegada estimada em %.1f min\n",
                   i, chegada[i].distancia_arestas, chegada[i].tempo_estimado_min);
        }
    }

    int *atingidos = malloc(g->num_vertices * sizeof(int));
    double limite_min = 10.0;
    int total_atingidos = listar_atingidos_ate(g, chegada, limite_min, atingidos);
    printf("\nVertices atingidos em ate %.0f minutos: %d\n", limite_min, total_atingidos);

    free(chegada);
    free(atingidos);
    destruir_grafo(g);
    return 0;
}