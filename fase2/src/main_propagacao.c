#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "geo.h"
#include "propagacao.h"
#include "log.h"

int main(int argc, char *argv[]) {
    const char *caminho = "data/mapa_brumadinho.geojson";
    if (argc > 1) {
        caminho = argv[1];
    }

    Grafo *g = carregar_grafo_geojson(caminho);
    if (!g) {
        fprintf(stderr, "Falha ao carregar o grafo.\n");
        return 1;
    }

    int origem = vertice_mais_proximo(g, BARRAGEM_LAT, BARRAGEM_LON);
    if (origem < 0) {
        fprintf(stderr, "Grafo sem vertices.\n");
        destruir_grafo(g);
        return 1;
    }
    printf("Ponto de ruptura: vertice %d (%.6f, %.6f)\n",
           origem, g->vertices[origem].lat, g->vertices[origem].lon);

    int *camada = log_malloc(sizeof(int) * g->num_vertices);

    Cronometro c;
    iniciar_cronometro(&c);
    int atingidos = simular_propagacao(g, origem, camada);
    double tempo_ms = parar_cronometro(&c);

    registrar_log("Propagacao_BFS", g->num_vertices, tempo_ms, memoria_pico_bytes());

    int ultima_camada = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        if (camada[i] > ultima_camada) ultima_camada = camada[i];
    }

    printf("Vertices atingidos: %d de %d\n", atingidos, g->num_vertices);
    printf("Vertices fora do alcance da lama: %d\n", g->num_vertices - atingidos);
    printf("Numero de camadas: %d (0 a %d)\n", ultima_camada + 1, ultima_camada);

    #if defined(_WIN32)
        system("if not exist saida mkdir saida");
    #else
        system("mkdir -p saida");
    #endif

    if (gravar_afetados_csv(g, camada, "saida/afetados.csv")) {
        printf("Arquivo gravado: saida/afetados.csv\n");
    }

    log_free(camada);
    destruir_grafo(g);
    return 0;
}
