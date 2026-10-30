#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "geo.h"
#include "subgrafos.h"
#include "log.h"

int main(int argc, char *argv[]) {
    const char *caminho_geojson = "data/mapa_brumadinho.geojson";
    if (argc > 1) {
        caminho_geojson = argv[1];
    }

    printf("Carregando o grafo principal (%s)...\n", caminho_geojson);
    
    Grafo *g = carregar_grafo_geojson(caminho_geojson);
    if (!g) {
        fprintf(stderr, "Erro ao carregar o GeoJSON do mapa.\n");
        return EXIT_FAILURE;
    }

    // Seleciona o ponto inicial da BFS (ex: vertice mais próximo da barragem)
    int origem = vertice_mais_proximo(g, BARRAGEM_LAT, BARRAGEM_LON);
    if (origem < 0) origem = 0;

    int tamanhos[] = {100, 500, 1000};
    int total_tamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]);

    Cronometro c;
    iniciar_cronometro(&c);

    for (int i = 0; i < total_tamanhos; i++) {
        char caminho_saida[128];
        snprintf(caminho_saida, sizeof(caminho_saida), "saida/subgrafo_%d.txt", tamanhos[i]);
        
        gerar_subgrafo_conexo(g, tamanhos[i], origem, caminho_saida);
    }

    double tempo_ms = parar_cronometro(&c);
    registrar_log("Gerador_Subgrafos", g->quantidade_vertices, tempo_ms, memoria_pico_bytes());

    destruir_grafo(g);
    printf("Todos os subgrafos foram gerados com sucesso!\n");
    return EXIT_SUCCESS;
}