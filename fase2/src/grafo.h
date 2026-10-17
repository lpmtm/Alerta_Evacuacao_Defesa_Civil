#ifndef GRAFO_H
#define GRAFO_H

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
    NUM_TIPOS_VIA
} TipoVia;

typedef struct {
    double lon;
    double lat;
} Coordenada;

typedef struct NoAdjacente {
    int destino;
    TipoVia tipo;
    struct NoAdjacente *prox;
} NoAdjacente;

typedef struct {
    int num_vertices;
    int capacidade;
    int num_arestas;

    Coordenada *vertices;

    NoAdjacente **lista_adj;
    int **matriz_adj;
    unsigned char **matriz_tipo;
} Grafo;

Grafo *criar_grafo(int capacidade_inicial);
void destruir_grafo(Grafo *g);

int obter_ou_criar_vertice(Grafo *g, double lon, double lat);

void adicionar_aresta(Grafo *g, int origem, int destino);
void adicionar_aresta_tipada(Grafo *g, int origem, int destino, TipoVia tipo);

TipoVia tipo_via_de_texto(const char *texto);
const char *nome_tipo_via(TipoVia tipo);

Grafo *carregar_grafo_geojson(const char *caminho_arquivo);

void imprimir_estatisticas(Grafo *g);
void imprimir_lista_adjacencia(Grafo *g);
void imprimir_matriz_adjacencia(Grafo *g, int limite);

#endif