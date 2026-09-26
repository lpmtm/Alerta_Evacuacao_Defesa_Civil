#ifndef TARJAN_H
#define TARJAN_H

#include "grafo.h"

typedef struct {
    int origem;
    int destino;
} Ponte;

typedef struct {
    Ponte *pontes;
    int quantidade_pontes;
    int capacidade_pontes;

    int *articulacoes;
    int quantidade_articulacoes;
    int capacidade_articulacoes;
} ResultadoTarjan;

void inicializar_resultado_tarjan(ResultadoTarjan *resultado);

void liberar_resultado_tarjan(ResultadoTarjan *resultado);

void executar_tarjan(
    Grafo *grafo,
    ResultadoTarjan *resultado
);

void imprimir_pontes(
    Grafo *grafo,
    ResultadoTarjan *resultado
);

void imprimir_articulacoes(
    Grafo *grafo,
    ResultadoTarjan *resultado
);

void identificar_vias_criticas(
    Grafo *grafo,
    ResultadoTarjan *resultado
);

void gerar_relatorio_vias_criticas(
    Grafo *grafo,
    ResultadoTarjan *resultado,
    const char *nome_arquivo
);

void identificar_vias_criticas(
    Grafo *grafo,
    ResultadoTarjan *resultado
);

void gerar_relatorio_vias_criticas(
    Grafo *grafo,
    ResultadoTarjan *resultado,
    const char *nome_arquivo
);

#endif