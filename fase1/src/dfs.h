#ifndef DFS_H
#define DFS_H

#include "grafo.h"

/*
 * Busca em Profundidade (issue #10).
 *
 * Percorre o grafo a partir de 'origem', indo o mais fundo possível
 * em cada ramo antes de retroceder (recursão - RNF01: sem pilha
 * pronta de biblioteca, a recursão do C é a "pilha" autoral aqui).
 *
 * Preenche, para cada vértice v de 0 a g->num_vertices - 1:
 *   predecessor[v] = vértice a partir do qual v foi descoberto
 *                    (-1 se v for a origem ou não for alcançável)
 *   tempo_entrada[v] = "timestamp" de quando v foi descoberto
 *   tempo_saida[v]   = "timestamp" de quando terminamos de explorar
 *                      todos os vizinhos de v (usado depois na
 *                      detecção de ciclos/bipartição, issue #12)
 *
 * O chamador aloca os três vetores com tamanho g->num_vertices.
 * Vértices não alcançados ficam com predecessor = -1 e
 * tempo_entrada = tempo_saida = -1.
 *
 * Registra tempo de execução e memória via log.h (RF03).
 * Retorna 1 em caso de sucesso, 0 se 'origem' for inválida.
 */
int dfs(Grafo *g, int origem, int *predecessor, int *tempo_entrada, int *tempo_saida);

#endif