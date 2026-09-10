#ifndef GRAFO_H
#define GRAFO_H

/*
 * Estrutura de Grafo para o projeto de Evacuação e Defesa Civil.
 * Suporta duas representações simultâneas: Lista de Adjacência e
 * Matriz de Adjacência, para permitir comparação de desempenho (RF02).
 *
 * Cada vértice representa um cruzamento (identificado por coordenadas
 * de latitude/longitude). Cada aresta representa um trecho de via
 * ligando dois cruzamentos.
 */

/* Coordenada geográfica de um vértice (cruzamento) */
typedef struct {
    double lon;
    double lat;
} Coordenada;

/* Nó da lista encadeada usada na Lista de Adjacência */
typedef struct NoAdjacente {
    int destino;
    struct NoAdjacente *prox;
} NoAdjacente;

/* Estrutura principal do grafo */
typedef struct {
    int num_vertices;          /* quantidade de vértices atualmente no grafo */
    int capacidade;            /* capacidade alocada (cresce conforme necessário) */
    int num_arestas;           /* quantidade de arestas inseridas */

    Coordenada *vertices;      /* vertices[i] = coordenada do vértice i */

    NoAdjacente **lista_adj;   /* lista_adj[i] = lista encadeada dos vizinhos de i */
    int **matriz_adj;          /* matriz_adj[i][j] = 1 se existe aresta i-j, 0 caso contrário */
} Grafo;

/* Cria um grafo vazio com uma capacidade inicial de vértices */
Grafo *criar_grafo(int capacidade_inicial);

/* Libera toda a memória alocada pelo grafo */
void destruir_grafo(Grafo *g);

/*
 * Procura um vértice pela coordenada (lon, lat).
 * Se não existir, cria um novo vértice.
 * Retorna o índice (id) do vértice.
 */
int obter_ou_criar_vertice(Grafo *g, double lon, double lat);

/* Adiciona uma aresta não-direcionada entre os vértices 'origem' e 'destino' */
void adicionar_aresta(Grafo *g, int origem, int destino);

/* Carrega um grafo a partir de um arquivo GeoJSON exportado do Overpass Turbo */
Grafo *carregar_grafo_geojson(const char *caminho_arquivo);

/* Imprime um resumo do grafo (número de vértices, arestas, etc.) */
void imprimir_estatisticas(Grafo *g);

#endif
