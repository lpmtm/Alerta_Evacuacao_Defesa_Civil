#ifndef COBERTURA_H
#define COBERTURA_H

#include <stdbool.h>
#include "grafo.h"

typedef struct {
    int *vertices;      // Array com os IDs dos vértices que compõem a cobertura
    int tamanho;       // Quantidade de vértices na cobertura mínima
} CoberturaVertices;

/**
 * Encontra a Cobertura de Vértices Mínima Exata utilizando Backtracking (Ramificação e Poda).
 * Retorna uma estrutura CoberturaVertices com o resultado obtido.
 */
CoberturaVertices resolver_cobertura_exata(Grafo *g);

/**
 * Libera a memória alocada pela estrutura CoberturaVertices.
 */
void liberar_cobertura(CoberturaVertices *cov);

#endif