#ifndef PROPAGACAO_H
#define PROPAGACAO_H

#include "grafo.h"

#define CAMADA_NAO_ATINGIDO -1

int simular_propagacao(Grafo *g, int origem, int *camada);
int gravar_afetados_csv(const Grafo *g, const int *camada, const char *caminho);

#endif
