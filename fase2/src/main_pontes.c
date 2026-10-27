#include "grafo.h"
#include "ponte.h"

#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

int main(int argc, char *argv[])
{
    const char *arquivo_geojson =
        argc > 1
            ? argv[1]
            : "data/mapa_brumadinho.geojson";

    const char *arquivo_csv = "saida/ponte.csv";
    const char *arquivo_txt = "saida/relatorio_pontes.txt";

    #if defined(_WIN32)
    _mkdir("saida");
    #else
    mkdir("saida", 0777);
    #endif

    Grafo grafo;
    inicializar_grafo(&grafo);

    printf("Carregando mapa: %s\n", arquivo_geojson);

    if (!carregar_geojson(&grafo, arquivo_geojson)) {
        fprintf(
            stderr,
            "Erro: não foi possível carregar as vias do GeoJSON.\n"
        );

        destruir_grafo_conteudo(&grafo);
        return EXIT_FAILURE;
    }

    printf("Vértices carregados: %d\n", grafo.quantidade_vertices);
    printf("Vias com metadados: %d\n", grafo.quantidade_vias);

    if (!gerar_relatorios_pontes(
            &grafo,
            arquivo_csv,
            arquivo_txt
        )) {
        fprintf(
            stderr,
            "Erro ao gerar os relatórios de pontes.\n"
        );

        destruir_grafo_conteudo(&grafo);
        return EXIT_FAILURE;
    }

    printf("Relatórios gerados:\n");
    printf(" - %s\n", arquivo_csv);
    printf(" - %s\n", arquivo_txt);

    destruir_grafo_conteudo(&grafo);

    return EXIT_SUCCESS;
}