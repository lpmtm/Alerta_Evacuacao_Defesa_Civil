#ifndef COMPONENTES_H
#define COMPONENTES_H

#include "grafo.h"

/*
 * Detecção de componentes conexos (issue #11).
 *
 * Roda BFS a partir de cada vértice ainda não visitado; cada rodada
 * descobre um componente conexo inteiro. Reaproveita o bfs() já
 * implementado na issue #9 (mesma fila autoral, mesmo log).
 *
 * Preenche componente[v] para cada vértice v de 0 a g->num_vertices-1
 * com o id do componente ao qual v pertence (0, 1, 2, ...).
 * O chamador aloca componente[] com tamanho g->num_vertices.
 *
 * Retorna o número total de componentes encontrados.
 */
int detectar_componentes(Grafo *g, int *componente);

/*
 * Lista os bairros (vértices) isolados: aqueles cujo componente tem
 * tamanho 1 (nenhuma conexão) ou que não pertencem ao maior
 * componente do grafo (o "componente principal").
 *
 * 'componente' e 'num_componentes' vêm de detectar_componentes().
 * Preenche 'isolados' (alocado pelo chamador com tamanho
 * g->num_vertices) com os índices dos vértices isolados.
 * Retorna quantos vértices isolados foram encontrados.
 */
int listar_isolados(Grafo *g, int *componente, int num_componentes, int *isolados);

#endif