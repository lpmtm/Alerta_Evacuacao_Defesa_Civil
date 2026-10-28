#ifndef PONTE_H
#define PONTE_H

#include "grafo.h"

int gerar_relatorios_pontes(
    Grafo *g,
    const char *caminho_csv,
    const char *caminho_txt
);

#endif