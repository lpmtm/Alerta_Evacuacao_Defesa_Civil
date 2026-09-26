#include <stdio.h>
#include <stdlib.h>

#include "tarjan.h"
#include "log.h"

static void adicionar_ponte(
    ResultadoTarjan *resultado,
    int origem,
    int destino
) {
    if (resultado->quantidade_pontes >=
        resultado->capacidade_pontes) {

        int nova_capacidade =
            resultado->capacidade_pontes == 0
                ? 10
                : resultado->capacidade_pontes * 2;

        resultado->pontes = log_realloc(
            resultado->pontes,
            nova_capacidade * sizeof(Ponte)
        );

        resultado->capacidade_pontes = nova_capacidade;
    }

    resultado->pontes[
        resultado->quantidade_pontes
    ].origem = origem;

    resultado->pontes[
        resultado->quantidade_pontes
    ].destino = destino;

    resultado->quantidade_pontes++;
}

static void adicionar_articulacao(
    ResultadoTarjan *resultado,
    int vertice
) {
    if (resultado->quantidade_articulacoes >=
        resultado->capacidade_articulacoes) {

        int nova_capacidade =
            resultado->capacidade_articulacoes == 0
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

    NoAdjacente *vizinho =
        grafo->lista_adj[vertice];

    while (vizinho != NULL) {

        int destino = vizinho->destino;

        if (destino == pai) {
            vizinho = vizinho->prox;
            continue;
        }

        if (descoberta[destino] == 0) {

            filhos++;

            dfs_tarjan(
                grafo,
                destino,
                vertice,
                descoberta,
                baixo,
                tempo,
                resultado
            );

            if (baixo[destino] < baixo[vertice]) {
                baixo[vertice] = baixo[destino];
            }

            if (baixo[destino] > descoberta[vertice]) {
                adicionar_ponte(
                    resultado,
                    vertice,
                    destino
                );
            }

            if (
                pai != -1 &&
                baixo[destino] >= descoberta[vertice] &&
                !ja_e_articulacao(resultado, vertice)
            ) {
                adicionar_articulacao(
                    resultado,
                    vertice
                );
            }

        } else {

            if (descoberta[destino] < baixo[vertice]) {
                baixo[vertice] = descoberta[destino];
            }
        }

        vizinho = vizinho->prox;
    }

    if (
        pai == -1 &&
        filhos >= 2 &&
        !ja_e_articulacao(resultado, vertice)
    ) {
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
    if (resultado == NULL) {
        return;
    }

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
    if (
        grafo == NULL ||
        resultado == NULL ||
        grafo->num_vertices <= 0
    ) {
        return;
    }

    int n = grafo->num_vertices;

    int *descoberta =
        log_malloc(n * sizeof(int));

    int *baixo =
        log_malloc(n * sizeof(int));

    if (
        descoberta == NULL ||
        baixo == NULL
    ) {
        if (descoberta != NULL) {
            log_free(descoberta);
        }

        if (baixo != NULL) {
            log_free(baixo);
        }

        return;
    }

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
    (void)grafo;

    printf("\n========== PONTES ==========\n");

    if (
        resultado == NULL ||
        resultado->quantidade_pontes == 0
    ) {
        printf("Nenhuma ponte encontrada.\n");
        return;
    }

    for (
        int i = 0;
        i < resultado->quantidade_pontes;
        i++
    ) {
        printf(
            "Ponte %d: %d -> %d\n",
            i + 1,
            resultado->pontes[i].origem,
            resultado->pontes[i].destino
        );
    }

    printf(
        "Total de pontes: %d\n",
        resultado->quantidade_pontes
    );
}

void imprimir_articulacoes(
    Grafo *grafo,
    ResultadoTarjan *resultado
) {
    (void)grafo;

    printf(
        "\n===== VERTICES DE ARTICULACAO =====\n"
    );

    if (
        resultado == NULL ||
        resultado->quantidade_articulacoes == 0
    ) {
        printf(
            "Nenhum vertice de articulacao encontrado.\n"
        );
        return;
    }

    for (
        int i = 0;
        i < resultado->quantidade_articulacoes;
        i++
    ) {
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
}

void identificar_vias_criticas(
    Grafo *grafo,
    ResultadoTarjan *resultado
) {
    (void)grafo;

    printf(
        "\n========== VIAS CRITICAS ==========\n"
    );

    if (
        resultado == NULL ||
        resultado->quantidade_pontes == 0
    ) {
        printf(
            "Nenhuma via critica identificada.\n"
        );
        return;
    }

    for (
        int i = 0;
        i < resultado->quantidade_pontes;
        i++
    ) {
        printf(
            "Via critica %d: %d -> %d\n",
            i + 1,
            resultado->pontes[i].origem,
            resultado->pontes[i].destino
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
    (void)grafo;

    if (
        resultado == NULL ||
        nome_arquivo == NULL
    ) {
        return;
    }

    FILE *arquivo =
        fopen(nome_arquivo, "w");

    if (arquivo == NULL) {
        printf(
            "Erro ao criar o relatorio: %s\n",
            nome_arquivo
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
        "===========================\n\n"
        "========================================\n\n"
    );

    fprintf(
        arquivo,
        "Total de pontes: %d\n",
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
        "-------------\n"
    );

    if (resultado->quantidade_pontes == 0) {

        fprintf(
            arquivo,
            "Nenhuma via critica identificada.\n"
        );

    } else {

        for (
            int i = 0;
            i < resultado->quantidade_pontes;
            i++
        ) {
            fprintf(
                arquivo,
                "Via %d: vertice %d -> vertice %d\n",
                i + 1,
                resultado->pontes[i].origem,
                resultado->pontes[i].destino
            );
        }
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
        "------------------------\n"
    );

    if (
        resultado->quantidade_articulacoes == 0
    ) {

        fprintf(
            arquivo,
            "Nenhum vertice de articulacao encontrado.\n"
        );

    } else {

        for (
            int i = 0;
            i < resultado->quantidade_articulacoes;
            i++
        ) {
            fprintf(
                arquivo,
                "Vertice %d: %d\n",
                i + 1,
                resultado->articulacoes[i]
            );
        }
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
        "\nANALISE\n"
    );

    fprintf(
        arquivo,
        "-------\n"
    );

    fprintf(
        arquivo,
        "As vias identificadas como pontes representam "
        "arestas cuja remocao pode aumentar o numero "
        "de componentes conexos do grafo.\n"
        "\nConclusao:\n"
    );

    fprintf(
        arquivo,
        "Essas vias devem ser consideradas na analise "
        "de rotas de fuga e no planejamento de reforco "
        "estrutural preventivo.\n"
        "As vias listadas devem ser consideradas "
        "na analise de reforco estrutural preventivo "
        "e planejamento de rotas de fuga.\n"
    );

    fclose(arquivo);
}