#ifndef PESOS_H
#define PESOS_H

#include "grafo.h"

double distancia_haversine(double lat1, double lon1, double lat2, double lon2);
double distancia_entre_vertices(const Grafo *g, int u, int v);

#endif
