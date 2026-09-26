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

int executar_tarjan(Grafo *g, ResultadoTarjan *resultado);

void imprimir_pontes(
    Grafo *g,
    const ResultadoTarjan *resultado
);

void imprimir_articulacoes(
    Grafo *g,
    const ResultadoTarjan *resultado
);

void gerar_relatorio_vias_criticas(
    Grafo *g,
    const ResultadoTarjan *resultado
);

void identificar_vias_criticas(
    Grafo *grafo,
    ResultadoTarjan *resultado
);

#endif