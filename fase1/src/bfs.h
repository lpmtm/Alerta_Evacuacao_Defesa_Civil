#ifndef BFS_H
#define BFS_H

#include "grafo.h"

#define BFS_INFINITO -1

/*
 * Busca em Largura (issue #9).
 *
 * Percorre o grafo a partir de 'origem' nível por nível, usando uma
 * fila (implementação autoral, sem bibliotecas prontas - RNF01).
 *
 * Preenche, para cada vértice v de 0 a g->num_vertices - 1:
 *   distancia[v]   = número mínimo de arestas de 'origem' até v
 *                    (BFS_INFINITO se v não for alcançável)
 *   predecessor[v] = vértice anterior a v no caminho encontrado
 *                    (-1 se v for a origem ou não for alcançável)
 *
 * O chamador é responsável por alocar distancia[] e predecessor[]
 * com tamanho g->num_vertices.
 *
 * Registra tempo de execução e memória via log.h (RF03).
 * Retorna 1 em caso de sucesso, 0 se 'origem' for inválida.
 */
int bfs(Grafo *g, int origem, int *distancia, int *predecessor);

#endif