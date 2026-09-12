#ifndef PROPAGACAO_H
#define PROPAGACAO_H

#include "grafo.h"

/*
 * Simulação de propagação da lama/rejeito (issue #13).
 *
 * Reaproveita o bfs() já implementado, o grafo ainda não
 * tem peso nas arestas, então cada nível do BFS a partir do ponto de
 * ruptura representa uma unidade de tempo (um "trecho" percorrido).
 *
 * ATENÇÃO: 'minutos_por_trecho' é um valor de referência/placeholder
 * (tempo médio estimado para a lama atravessar um trecho de via).
 * Quando a Fase II migrar o grafo para arestas com peso (distância
 * ou velocidade real), o ideal é trocar isso por uma versão baseada
 * no peso de cada aresta, sem precisar mudar a assinatura desta
 * função - alinhar com quem estiver na Fase II antes de duplicar
 * esse trabalho.
 */
typedef struct {
    int distancia_arestas;    /* nível do BFS (BFS_INFINITO se não atingido) */
    double tempo_estimado_min; /* distancia_arestas * minutos_por_trecho */
} ChegadaLama;

/*
 * Simula a propagação a partir do vértice 'origem' (ponto de ruptura
 * da barragem). Preenche 'chegada' (alocado pelo chamador com
 * tamanho g->num_vertices) com a distância em trechos e o tempo
 * estimado de chegada da lama em cada vértice.
 * Retorna 1 em caso de sucesso, 0 se 'origem' for inválida.
 */
int simular_propagacao(Grafo *g, int origem, double minutos_por_trecho, ChegadaLama *chegada);

/*
 * Lista os vértices atingidos pela lama até 'tempo_limite_min'
 * minutos - útil para a Defesa Civil definir a zona de evacuação
 * imediata. Preenche 'atingidos' (alocado pelo chamador com tamanho
 * g->num_vertices). Retorna quantos vértices foram atingidos até o
 * limite.
 */
int listar_atingidos_ate(Grafo *g, ChegadaLama *chegada, double tempo_limite_min, int *atingidos);

#endif