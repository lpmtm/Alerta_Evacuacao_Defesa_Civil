#include <stdio.h>
#include "tarjan.h"
#include "log.h"

static int garantir_capacidade_pontes(
    ResultadoTarjan *resultado
) {
    if (resultado->quantidade_pontes <
        resultado->capacidade_pontes) {

        return 1;
    }

    int nova_capacidade;

    if (resultado->capacidade_pontes == 0) {
        nova_capacidade = 16;
    } else {
        nova_capacidade =
            resultado->capacidade_pontes * 2;
    }

    Ponte *nova = (Ponte *)log_realloc(
        resultado->pontes,
        (size_t)nova_capacidade * sizeof(Ponte)
    );

    if (!nova) {
        return 0;
    }

    resultado->pontes = nova;
    resultado->capacidade_pontes = nova_capacidade;

    return 1;
}

static int adicionar_ponte(
    ResultadoTarjan *resultado,
    int origem,
    int destino
) {
    if (!garantir_capacidade_pontes(resultado)) {
        return 0;
    }

    resultado->pontes[
        resultado->quantidade_pontes
    ].origem = origem;

    resultado->pontes[
        resultado->quantidade_pontes
    ].destino = destino;

    resultado->quantidade_pontes++;

    return 1;
}

static int garantir_capacidade_articulacoes(
    ResultadoTarjan *resultado
) {
    if (resultado->quantidade_articulacoes <
        resultado->capacidade_articulacoes) {

        return 1;
    }

    int nova_capacidade;

    if (resultado->capacidade_articulacoes == 0) {
        nova_capacidade = 16;
    } else {
        nova_capacidade =
            resultado->capacidade_articulacoes * 2;
    }

    int *nova = (int *)log_realloc(
        resultado->articulacoes,
        (size_t)nova_capacidade * sizeof(int)
    );

    if (!nova) {
        return 0;
    }

    resultado->articulacoes = nova;
    resultado->capacidade_articulacoes =
        nova_capacidade;

    return 1;
}

static int eh_articulacao(
    const ResultadoTarjan *resultado,
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

static int adicionar_articulacao(
    ResultadoTarjan *resultado,
    int vertice
) {
    if (eh_articulacao(resultado, vertice)) {
        return 1;
    }

    if (!garantir_capacidade_articulacoes(resultado)) {
        return 0;
    }

    resultado->articulacoes[
        resultado->quantidade_articulacoes
    ] = vertice;

    resultado->quantidade_articulacoes++;

    return 1;
}

static int tarjan_dfs(
    Grafo *g,
    int vertice,
    int pai,
    int *tempo,
    int *discovery,
    int *low,
    int *pais,
    ResultadoTarjan *resultado
) {

    discovery[vertice] = *tempo;

    low[vertice] = *tempo;

    (*tempo)++;

    int filhos = 0;

    int eh_raiz = (pai == -1);

    NoAdjacente *atual =
        g->lista_adj[vertice];

    while (atual) {

        int vizinho = atual->destino;

        if (discovery[vizinho] == -1) {

            pais[vizinho] = vertice;

            filhos++;

            if (!tarjan_dfs(
                    g,
                    vizinho,
                    vertice,
                    tempo,
                    discovery,
                    low,
                    pais,
                    resultado
                )) {

                return 0;
            }

            if (low[vizinho] < low[vertice]) {

                low[vertice] =
                    low[vizinho];
            }


            if (low[vizinho] >
                discovery[vertice]) {

                if (!adicionar_ponte(
                        resultado,
                        vertice,
                        vizinho
                    )) {

                    return 0;
                }
            }


            if (!eh_raiz &&
                low[vizinho] >=
                discovery[vertice]) {

                if (!adicionar_articulacao(
                        resultado,
                        vertice
                    )) {

                    return 0;
                }
            }

        }

        else if (vizinho != pais[vertice]) {

            if (discovery[vizinho] <
                low[vertice]) {

                low[vertice] =
                    discovery[vizinho];
            }
        }


        atual = atual->prox;
    }

    if (eh_raiz && filhos >= 2) {

        if (!adicionar_articulacao(
                resultado,
                vertice
            )) {

            return 0;
        }
    }

    return 1;
}

void inicializar_resultado_tarjan(
    ResultadoTarjan *resultado
) {
    if (!resultado) {
        return;
    }

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
    if (!resultado) {
        return;
    }

    log_free(resultado->pontes);
    log_free(resultado->articulacoes);

    resultado->pontes = NULL;
    resultado->articulacoes = NULL;

    resultado->quantidade_pontes = 0;
    resultado->quantidade_articulacoes = 0;

    resultado->capacidade_pontes = 0;
    resultado->capacidade_articulacoes = 0;
}

int executar_tarjan(
    Grafo *g,
    ResultadoTarjan *resultado
) {
    if (!g ||
        !resultado ||
        g->num_vertices <= 0) {

        return 0;
    }


    inicializar_resultado_tarjan(resultado);


    int n = g->num_vertices;

    int *discovery =
        (int *)log_malloc(
            (size_t)n * sizeof(int)
        );

    int *low =
        (int *)log_malloc(
            (size_t)n * sizeof(int)
        );

    int *pais =
        (int *)log_malloc(
            (size_t)n * sizeof(int)
        );


    if (!discovery ||
        !low ||
        !pais) {

        log_free(discovery);
        log_free(low);
        log_free(pais);

        return 0;
    }

    for (int i = 0; i < n; i++) {

        discovery[i] = -1;
        low[i] = -1;
        pais[i] = -1;
    }


    int tempo = 0;

    int sucesso = 1;

    for (int i = 0; i < n; i++) {

        if (discovery[i] == -1) {

            if (!tarjan_dfs(
                    g,
                    i,
                    -1,
                    &tempo,
                    discovery,
                    low,
                    pais,
                    resultado
                )) {

                sucesso = 0;
                break;
            }
        }
    }

    log_free(discovery);
    log_free(low);
    log_free(pais);


    if (!sucesso) {

        liberar_resultado_tarjan(resultado);

        return 0;
    }


    return 1;
}

void imprimir_pontes(
    Grafo *g,
    const ResultadoTarjan *resultado
) {
    if (!g || !resultado) {
        return;
    }


    printf("\n");
    printf("===== PONTES ENCONTRADAS =====\n");

    printf(
        "Quantidade: %d\n",
        resultado->quantidade_pontes
    );


    if (resultado->quantidade_pontes == 0) {

        printf(
            "Nenhuma ponte encontrada.\n"
        );

        printf(
            "===============================\n"
        );

        return;
    }


    for (int i = 0;
         i < resultado->quantidade_pontes;
         i++) {

        int origem =
            resultado->pontes[i].origem;

        int destino =
            resultado->pontes[i].destino;


        printf(
            "\nPonte %d: %d <-> %d\n",
            i + 1,
            origem,
            destino
        );


        printf(
            "  Origem : lon=%.6f, lat=%.6f\n",
            g->vertices[origem].lon,
            g->vertices[origem].lat
        );


        printf(
            "  Destino: lon=%.6f, lat=%.6f\n",
            g->vertices[destino].lon,
            g->vertices[destino].lat
        );
    }


    printf(
        "\n===============================\n"
    );
}

void imprimir_articulacoes(
    Grafo *g,
    const ResultadoTarjan *resultado
) {
    if (!g || !resultado) {
        return;
    }


    printf("\n");
    printf(
        "===== VERTICES DE ARTICULACAO =====\n"
    );


    printf(
        "Quantidade: %d\n",
        resultado->quantidade_articulacoes
    );


    if (resultado->quantidade_articulacoes == 0) {

        printf(
            "Nenhum vertice de articulacao encontrado.\n"
        );

        printf(
            "===================================\n"
        );

        return;
    }


    for (int i = 0;
         i < resultado->quantidade_articulacoes;
         i++) {

        int vertice =
            resultado->articulacoes[i];


        printf(
            "\nArticulacao %d: vertice %d\n",
            i + 1,
            vertice
        );


        printf(
            "  Coordenadas: lon=%.6f, lat=%.6f\n",
            g->vertices[vertice].lon,
            g->vertices[vertice].lat
        );
    }


    printf(
        "\n===================================\n"
    );
}


void gerar_relatorio_vias_criticas(
    Grafo *g,
    const ResultadoTarjan *resultado
) {
    if (!g || !resultado) {
        return;
    }


    printf("\n");
    printf(
        "==============================================\n"
    );

    printf(
        "       RELATORIO DE VIAS CRITICAS\n"
    );

    printf(
        "==============================================\n"
    );


    printf(
        "Quantidade de pontes: %d\n",
        resultado->quantidade_pontes
    );


    printf(
        "Quantidade de vertices de articulacao: %d\n",
        resultado->quantidade_articulacoes
    );

    printf(
        "\n--- VIAS CRITICAS (PONTES) ---\n"
    );


    if (resultado->quantidade_pontes == 0) {

        printf(
            "Nenhuma via critica identificada como ponte.\n"
        );

    } else {

        for (int i = 0;
             i < resultado->quantidade_pontes;
             i++) {

            int origem =
                resultado->pontes[i].origem;

            int destino =
                resultado->pontes[i].destino;


            printf(
                "\nVia critica %d\n",
                i + 1
            );


            printf(
                "  Vertices: %d <-> %d\n",
                origem,
                destino
            );


            printf(
                "  Origem : lon=%.6f, lat=%.6f\n",
                g->vertices[origem].lon,
                g->vertices[origem].lat
            );


            printf(
                "  Destino: lon=%.6f, lat=%.6f\n",
                g->vertices[destino].lon,
                g->vertices[destino].lat
            );
        }
    }

    printf(
        "\n--- PONTOS DE CONEXAO CRITICOS ---\n"
    );


    if (resultado->quantidade_articulacoes == 0) {

        printf(
            "Nenhum vertice de articulacao identificado.\n"
        );

    } else {

        for (int i = 0;
             i < resultado->quantidade_articulacoes;
             i++) {

            int vertice =
                resultado->articulacoes[i];


            printf(
                "\nPonto critico %d\n",
                i + 1
            );


            printf(
                "  Vertice: %d\n",
                vertice
            );


            printf(
                "  Coordenadas: lon=%.6f, lat=%.6f\n",
                g->vertices[vertice].lon,
                g->vertices[vertice].lat
            );
        }
    }


    printf(
        "\nObservacao: as pontes sao as conexoes "
        "estruturalmente criticas\n"
    );

    printf(
        "identificadas pelo algoritmo de Tarjan "
        "no grafo fornecido.\n"
    );


    printf(
        "==============================================\n"
    );
}