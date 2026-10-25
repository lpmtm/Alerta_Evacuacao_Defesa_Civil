#ifndef GRAFO_H
#define GRAFO_H

#include <stddef.h>
typedef enum {
    VIA_DESCONHECIDA = 0,
    VIA_RESIDENCIAL,
    VIA_PRINCIPAL,
    VIA_SECUNDARIA,
    VIA_TERCIARIA,
    VIA_LOCAL,
    VIA_SERVICO,
    VIA_TRILHA,
    VIA_OUTRA,

    /* Compatibilidade com os nomes usados pelo módulo de pesos. */
    VIA_SERVICE = VIA_SERVICO,
    VIA_TRACK = VIA_TRILHA,
    VIA_TERTIARY = VIA_TERCIARIA,
    VIA_TERTIARY_LINK = VIA_SECUNDARIA,
    VIA_UNCLASSIFIED = VIA_LOCAL,
    VIA_PATH = VIA_TRILHA,
    VIA_FOOTWAY = VIA_TRILHA,
    NUM_TIPOS_VIA = VIA_OUTRA + 1
} TipoVia;
typedef struct {
    double lon;
    double lat;
} Coordenada;

typedef struct {
    char *id;
    char *nome;
    char *nome_oficial;
    char *ref;
} MetadadosVia;

typedef struct NoAdjacente {
    int destino;
    int id_via;
    int id_aresta;
    TipoVia tipo;
    struct NoAdjacente *prox;
} NoAdjacente;

typedef struct {
    Coordenada coordenada;
    NoAdjacente *adjacencias;
} Vertice;

typedef struct {
    Vertice *vertices;
    int quantidade_vertices;
    int capacidade_vertices;

    MetadadosVia *metadados_vias;
    int quantidade_vias;
    int capacidade_vias;

    int quantidade_arestas;
} Grafo;

void inicializar_grafo(Grafo *g);
void destruir_grafo(Grafo *g);

int carregar_geojson(Grafo *g, const char *caminho);

int obter_ou_criar_vertice(
    Grafo *g,
    double lon,
    double lat
);

int adicionar_aresta_com_via(
    Grafo *g,
    int origem,
    int destino,
    TipoVia tipo,
    int id_via
);

TipoVia tipo_via_de_texto(const char *texto);
const char *nome_tipo_via(TipoVia tipo);

#endif