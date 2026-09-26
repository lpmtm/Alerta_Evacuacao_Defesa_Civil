#include <stdio.h>
#include <stdlib.h>

#include "tarjan.h"
#include "log.h"
static void adicionar_ponte(ResultadoTarjan *resultado, int origem, int destino) {
    if (resultado->quantidade_pontes >= resultado->capacidade_pontes) {
        int nova_capacidade =
            (resultado->capacidade_pontes == 0)
                ? 10
                : resultado->capacidade_pontes * 2;

        resultado->pontes = log_realloc(
            resultado->pontes,
            nova_capacidade * sizeof(Ponte)
        );

        resultado->capacidade_pontes = nova_capacidade;
    }

    resultado->pontes[resultado->quantidade_pontes].origem = origem;
    resultado->pontes[resultado->quantidade_pontes].destino = destino;

    resultado->quantidade_pontes++;
}

static void adicionar_articulacao(
    ResultadoTarjan *resultado,
    int vertice
) {
    if (resultado->quantidade_articulacoes >=
        resultado->capacidade_articulacoes) {

        int nova_capacidade =
            (resultado->capacidade_articulacoes == 0)
                ? 10
                : resultado->capacidade_articulacoes * 2;

        resultado->articulacoes = log_realloc(
            resultado->articulacoes,
            nova_capacidade * sizeof(int)
        );

        resultado->capacidade_articulacoes = nova_capacidade;
    }

    resultado->articulacoes[
        resultado->quantidade_articulacoes
    ] = vertice;

    resultado->quantidade_articulacoes++;
}

static int ja_e_articulacao(
    ResultadoTarjan *resultado,
    int vertice
) {
    for (int i = 0;
         i < resultado->quantidade_articulacoes;
         i++) {

        if (resultado->articulacoes[i] == vertice) {
            return 1;
        }
    }

    return 0;
}

static void dfs_tarjan(
    Grafo *grafo,
    int vertice,
    int pai,
    int *descoberta,
    int *baixo,
    int *tempo,
    ResultadoTarjan *resultado
) {
    descoberta[vertice] = ++(*tempo);
    baixo[vertice] = descoberta[vertice];

    int filhos = 0;

    NoAdjacente *vizinho = grafo->lista_adj[vertice];

    if (pai == -1 &&
        filhos >= 2 &&
        !ja_e_articulacao(resultado, vertice)) {

        adicionar_articulacao(
            resultado,
            vertice
        );
    }
}

void inicializar_resultado_tarjan(
    ResultadoTarjan *resultado
) {
    resultado->pontes = NULL;
    resultado->quantidade_pontes = 0;
    resultado->capacidade_pontes = 0;

    resultado->articulacoes = NULL;
    resultado->quantidade_articulacoes = 0;
    resultado->capacidade_articulacoes = 0;
}

void liberar_resultado_tarjan(
    ResultadoTarjan *resultado
) {
    if (resultado->pontes != NULL) {
        log_free(resultado->pontes);
        resultado->pontes = NULL;
    }

    if (resultado->articulacoes != NULL) {
        log_free(resultado->articulacoes);
        resultado->articulacoes = NULL;
    }

    resultado->quantidade_pontes = 0;
    resultado->capacidade_pontes = 0;

    resultado->quantidade_articulacoes = 0;
    resultado->capacidade_articulacoes = 0;
}

void executar_tarjan(
    Grafo *grafo,
    ResultadoTarjan *resultado
) {
    if (grafo == NULL || resultado == NULL) {
        return;
    }

    int n = grafo->num_vertices;

    int *descoberta = log_malloc(n * sizeof(int));
    int *baixo = log_malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        descoberta[i] = 0;
        baixo[i] = 0;
    }

    int tempo = 0;

    for (int i = 0; i < n; i++) {

        if (descoberta[i] == 0) {

            dfs_tarjan(
                grafo,
                i,
                -1,
                descoberta,
                baixo,
                &tempo,
                resultado
            );
        }
    }

    log_free(descoberta);
    log_free(baixo);
}

void imprimir_pontes(
    Grafo *grafo,
    ResultadoTarjan *resultado
) {
    printf("\n========== PONTES ==========\n");

    if (resultado->quantidade_pontes == 0) {
        printf("Nenhuma ponte encontrada.\n");
        return;
    }

    for (int i = 0;
         i < resultado->quantidade_pontes;
         i++) {

        int origem = resultado->pontes[i].origem;
        int destino = resultado->pontes[i].destino;

        printf(
            "Ponte %d: %d -> %d\n",
            i + 1,
            origem,
            destino
        );
    }

    printf(
        "Total de pontes: %d\n",
        resultado->quantidade_pontes
    );

    (void)grafo;
}

void imprimir_articulacoes(
    Grafo *grafo,
    ResultadoTarjan *resultado
) {
    printf("\n===== VERTICES DE ARTICULACAO =====\n");

    if (resultado->quantidade_articulacoes == 0) {
        printf("Nenhum vertice de articulacao encontrado.\n");
        return;
    }

    for (int i = 0;
         i < resultado->quantidade_articulacoes;
         i++) {

        printf(
            "Vertice de articulacao %d: %d\n",
            i + 1,
            resultado->articulacoes[i]
        );
    }

    printf(
        "Total de vertices de articulacao: %d\n",
        resultado->quantidade_articulacoes
    );

    (void)grafo;

}

void identificar_vias_criticas(
    Grafo *grafo,
    ResultadoTarjan *resultado
) {
    if (grafo == NULL || resultado == NULL) {
        return;
    }

    printf("\n========== VIAS CRITICAS ==========\n");

    if (resultado->quantidade_pontes == 0) {
        printf("Nenhuma via critica identificada.\n");
        return;
    }

    for (int i = 0;
         i < resultado->quantidade_pontes;
         i++) {

        int origem = resultado->pontes[i].origem;
        int destino = resultado->pontes[i].destino;

        printf(
            "Via critica %d: %d -> %d\n",
            i + 1,
            origem,
            destino
        );
    }

    printf(
        "Total de vias criticas: %d\n",
        resultado->quantidade_pontes
    );
}

void gerar_relatorio_vias_criticas(
    Grafo *grafo,
    ResultadoTarjan *resultado,
    const char *nome_arquivo
) {
    if (grafo == NULL ||
        resultado == NULL ||
        nome_arquivo == NULL) {
        return;
    }

    FILE *arquivo = fopen(nome_arquivo, "w");

    if (arquivo == NULL) {
        printf(
            "Erro ao criar o arquivo de relatorio.\n"
        );
        return;
    }

    fprintf(
        arquivo,
        "========================================\n"
    );

    fprintf(
        arquivo,
        "RELATORIO DE VIAS CRITICAS\n"
    );

    fprintf(
        arquivo,
        "========================================\n\n"
    );

    fprintf(
        arquivo,
        "Total de pontes identificadas: %d\n",
        resultado->quantidade_pontes
    );

    fprintf(
        arquivo,
        "Total de vertices de articulacao: %d\n\n",
        resultado->quantidade_articulacoes
    );

    fprintf(
        arquivo,
        "VIAS CRITICAS\n"
    );

    fprintf(
        arquivo,
        "----------------------------------------\n"
    );

    for (int i = 0;
         i < resultado->quantidade_pontes;
         i++) {

        int origem =
            resultado->pontes[i].origem;

        int destino =
            resultado->pontes[i].destino;

        fprintf(
            arquivo,
            "Via %d: vertice %d -> vertice %d\n",
            i + 1,
            origem,
            destino
        );
    }

    fprintf(
        arquivo,
        "\nVERTICES DE ARTICULACAO\n"
    );

    fprintf(
        arquivo,
        "----------------------------------------\n"
    );

    for (int i = 0;
         i < resultado->quantidade_articulacoes;
         i++) {

        fprintf(
            arquivo,
            "Vertice %d: %d\n",
            i + 1,
            resultado->articulacoes[i]
        );
    }

    fprintf(
        arquivo,
        "\nConclusao:\n"
    );

    fprintf(
        arquivo,
        "As vias listadas devem ser consideradas "
        "na analise de reforco estrutural preventivo "
        "e planejamento de rotas de fuga.\n"
    );

    fclose(arquivo);
}