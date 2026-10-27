
#ifndef GRAFO_H
#define GRAFO_H

#include <stddef.h>


typedef enum {
    VIA_DESCONHECIDA = 0,
    VIA_SERVICE,
    VIA_TRACK,
    VIA_TERTIARY,
    VIA_TERTIARY_LINK,
    VIA_UNCLASSIFIED,
    VIA_RESIDENTIAL,
    VIA_PATH,
    VIA_FOOTWAY,
    VIA_OUTRO,

    /* Categorias adicionais usadas pelo carregador novo. */
    VIA_PRINCIPAL,
    VIA_SECUNDARIA,
    VIA_LOCAL,

    /* Nomes compatíveis com a implementação nova. */
    VIA_SERVICO = VIA_SERVICE,
    VIA_TRILHA = VIA_PATH,
    VIA_TERCIARIA = VIA_TERTIARY,
    VIA_RESIDENCIAL = VIA_RESIDENTIAL,
    VIA_OUTRA = VIA_OUTRO,

    NUM_TIPOS_VIA = VIA_LOCAL + 1
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
    TipoVia tipo;
    int id_via;
    int id_aresta;
    struct NoAdjacente *prox;
} NoAdjacente;

typedef struct Vertice {
    double lon;
    double lat;
    NoAdjacente *adjacencias;
} Vertice;

typedef struct {
    union {
        int quantidade_vertices;
        int num_vertices;
    };

    union {
        int capacidade_vertices;
        int capacidade;
    };

    union {
        int quantidade_arestas;
        int num_arestas;
    };

    Vertice *vertices;

    MetadadosVia *metadados_vias;
    int quantidade_vias;
    int capacidade_vias;
} Grafo;

/* API de gerenciamento */
void inicializar_grafo(Grafo *g);
void destruir_grafo_conteudo(Grafo *g);
void destruir_grafo(Grafo *g);

/* API usada pelo carregador novo */
int carregar_geojson(Grafo *g, const char *caminho);
int obter_ou_criar_vertice(Grafo *g, double lon, double lat);

int adicionar_aresta_com_via(
    Grafo *g,
    int origem,
    int destino,
    TipoVia tipo,
    int id_via
);

/* API antiga mantida para os módulos existentes */
Grafo *criar_grafo(int capacidade_inicial);
Grafo *carregar_grafo_geojson(const char *caminho);
void adicionar_aresta(Grafo *g, int origem, int destino);
void adicionar_aresta_tipada(
    Grafo *g,
    int origem,
    int destino,
    TipoVia tipo
);

void imprimir_estatisticas(Grafo *g);
void imprimir_lista_adjacencia(Grafo *g);
void imprimir_matriz_adjacencia(Grafo *g, int limite);

/* Conversão de tipos de via */
TipoVia tipo_via_de_texto(const char *texto);
const char *nome_tipo_via(TipoVia tipo);

#endif
