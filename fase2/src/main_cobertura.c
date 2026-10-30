#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "cobertura.h"
#include "log.h"

int main(int argc, char *argv[]) {
    const char *caminho_subgrafo = "saida/subgrafo_100.txt";
    if (argc > 1) {
        caminho_subgrafo = argv[1];
    }

    printf("Carregando o subgrafo (%s)...\n", caminho_subgrafo);

    // Carrega o subgrafo gerado na Issue #52
    Grafo *g = carregar_grafo_geojson(caminho_subgrafo);
    if (!g) {
        fprintf(stderr, "Erro ao carregar o ficheiro de subgrafo.\n");
        return EXIT_FAILURE;
    }

    printf("Executando Cobertura de Vértices Exata (Backtracking) para %d vértices...\n", g->quantidade_vertices);

    Cronometro c;
    iniciar_cronometro(&c);

    CoberturaVertices cov = resolver_cobertura_exata(g);

    double tempo_ms = parar_cronometro(&c);
    registrar_log("Cobertura_Exata_Backtracking", g->quantidade_vertices, tempo_ms, memoria_pico_bytes());

    printf("\n--- RESULTADO DA COBERTURA DE VÉRTICES (EXATA) ---\n");
    printf("Postos de atendimento necessários (Tamanho da Cobertura): %d\n", cov.tamanho);
    printf("Tempo de Execução: %.3f ms\n", tempo_ms);
    printf("Memória Pico: %ld bytes\n", memoria_pico_bytes());

    #if defined(_WIN32)
        system("if not exist saida mkdir saida");
    #else
        system("mkdir -p saida");
    #endif

    FILE *f = fopen("saida/cobertura_exata.csv", "w");
    if (f) {
        fprintf(f, "id_vertice,lat,lon\n");
        for (int i = 0; i < cov.tamanho; i++) {
            int idx = cov.vertices[i];
            fprintf(f, "%d,%.6f,%.6f\n", idx, g->vertices[idx].lat, g->vertices[idx].lon);
        }
        fclose(f);
        printf("Relatório salvo em 'saida/cobertura_exata.csv'.\n");
    }

    liberar_cobertura(&cov);
    destruir_grafo(g);

    return EXIT_SUCCESS;
}