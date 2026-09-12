#ifndef CICLOS_H
#define CICLOS_H

#include "grafo.h"

/*
 * Detecção de ciclos e verificação de bipartição (issue #12).
 *
 * As duas análises saem da mesma passada por BFS (fila autoral):
 *
 * - Ciclo: ao visitar os vizinhos de um vértice já colorido, se um
 *   vizinho já tem cor atribuída e NÃO é o pai direto na busca,
 *   encontramos uma aresta de retorno -> o grafo tem ciclo.
 *
 * - Bipartição: colorimos os vértices com 2 cores (0/1) alternando
 *   por nível a partir de cada origem. Se algum vizinho tiver a
 *   mesma cor do vértice atual, o grafo não é bipartido.
 *
 * Como o grafo pode ter mais de um componente conexo (ver issue
 * #11), a busca é repetida a partir de cada vértice ainda não
 * colorido, cobrindo o grafo inteiro.
 */
typedef struct {
    int tem_ciclo;
    int eh_bipartido;
} ResultadoCiclosBipartido;

/*
 * Roda a análise no grafo inteiro. Preenche cor[] (0 ou 1) para
 * cada vértice - útil, por exemplo, para colorir o mapa em duas
 * regiões alternadas caso o grafo seja bipartido.
 * O chamador aloca cor[] com tamanho g->num_vertices.
 */
ResultadoCiclosBipartido detectar_ciclos_bipartido(Grafo *g, int *cor);

#endif