#include <stdio.h>

#include "grafo.h"
#include "tarjan.h"

int main(void) {

    Grafo *grafo = NULL;
    ResultadoTarjan resultado;

    inicializar_resultado_tarjan(&resultado);

    printf("========================================\n");
    printf("       TESTE DO ALGORITMO DE TARJAN\n");
    printf("========================================\n");

    printf("\nCarregando mapa de Brumadinho...\n");

    grafo = carregar_grafo_geojson(
        "../data/mapa_brumadinho.geojson"
    );

    if (grafo == NULL) {
        printf("\nERRO: Nao foi possivel carregar o grafo.\n");

        liberar_resultado_tarjan(&resultado);

        return 1;
    }

    printf("\nGrafo carregado com sucesso!\n");
    printf("Quantidade de vertices: %d\n", grafo->num_vertices);
    printf("Quantidade de arestas: %d\n", grafo->num_arestas);

    printf("\nExecutando algoritmo de Tarjan...\n");

    executar_tarjan(
        grafo,
        &resultado
    );

    printf("\nAlgoritmo de Tarjan executado com sucesso!\n");

    imprimir_pontes(
        grafo,
        &resultado
    );

    imprimir_articulacoes(
        grafo,
        &resultado
    );

    identificar_vias_criticas(
        grafo,
        &resultado
    );

    printf("\nGerando relatorio...\n");

    gerar_relatorio_vias_criticas(
        grafo,
        &resultado,
        "../data/relatorio_vias_criticas.txt"
    );

    printf("\nRelatorio gerado com sucesso!\n");
    printf("Arquivo: ../data/relatorio_vias_criticas.txt\n");

    printf("\nLiberando memoria...\n");

    liberar_resultado_tarjan(
        &resultado
    );

    destruir_grafo(
        grafo
    );

    printf("\n========================================\n");
    printf("           TESTE FINALIZADO\n");
    printf("========================================\n");

    return 0;
}