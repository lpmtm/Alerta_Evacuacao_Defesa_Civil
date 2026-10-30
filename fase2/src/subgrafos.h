#ifndef SUBGRAFOS_H
#define SUBGRAFOS_H

#include "grafo.h"

/**
 * Extrai um subgrafo conexo com exatamente 'n_vertices' a partir de 'vertice_inicial'
 * no grafo de origem usando uma Busca em Largura (BFS).
 * Salva a lista de vértices e as arestas induzidas num arquivo de texto.
 */
void gerar_subgrafo_conexo(Grafo *g_origem, int n_vertices, int vertice_inicial, const char *ficheiro_saida);

#endif