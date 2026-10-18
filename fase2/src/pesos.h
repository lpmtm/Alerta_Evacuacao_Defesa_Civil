#ifndef PESOS_H
#define PESOS_H

#include "grafo.h"

double distancia_haversine(double lat1, double lon1, double lat2, double lon2);
double distancia_entre_vertices(const Grafo *g, int u, int v);

double velocidade_via_kmh(TipoVia tipo);
double fator_lentidao(TipoVia tipo);
void imprimir_tabela_lentidao(void);

#endif