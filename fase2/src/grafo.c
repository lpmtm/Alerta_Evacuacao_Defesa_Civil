#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "grafo.h"

#define EPSILON 0.0000001

static const char *NOMES_TIPO_VIA[NUM_TIPOS_VIA] = {
    "desconhecida",
    "service",
    "track",
    "tertiary",
    "tertiary_link",
    "unclassified",
    "residential",
    "path",
    "footway",
    "outro"
};

TipoVia tipo_via_de_texto(const char *texto) {
    if (strcmp(texto, "service") == 0) return VIA_SERVICE;
    if (strcmp(texto, "track") == 0) return VIA_TRACK;
    if (strcmp(texto, "tertiary") == 0) return VIA_TERTIARY;
    if (strcmp(texto, "tertiary_link") == 0) return VIA_TERTIARY_LINK;
    if (strcmp(texto, "unclassified") == 0) return VIA_UNCLASSIFIED;
    if (strcmp(texto, "residential") == 0) return VIA_RESIDENTIAL;
    if (strcmp(texto, "path") == 0) return VIA_PATH;
    if (strcmp(texto, "footway") == 0) return VIA_FOOTWAY;
    return VIA_OUTRO;
}

const char *nome_tipo_via(TipoVia tipo) {
    if (tipo < 0 || tipo >= NUM_TIPOS_VIA) return NOMES_TIPO_VIA[VIA_DESCONHECIDA];
    return NOMES_TIPO_VIA[tipo];
}

Grafo *criar_grafo(int capacidade_inicial) {
    Grafo *g = malloc(sizeof(Grafo));
    if (!g) {
        fprintf(stderr, "Erro ao alocar grafo.\n");
        exit(1);
    }

    g->num_vertices = 0;
    g->num_arestas = 0;
    g->capacidade = capacidade_inicial;

    g->vertices = malloc(sizeof(Coordenada) * capacidade_inicial);
    g->lista_adj = calloc(capacidade_inicial, sizeof(NoAdjacente *));

    g->matriz_adj = malloc(sizeof(int *) * capacidade_inicial);
    g->matriz_tipo = malloc(sizeof(unsigned char *) * capacidade_inicial);
    for (int i = 0; i < capacidade_inicial; i++) {
        g->matriz_adj[i] = calloc(capacidade_inicial, sizeof(int));
        g->matriz_tipo[i] = calloc(capacidade_inicial, sizeof(unsigned char));
    }

    return g;
}

void destruir_grafo(Grafo *g) {
    if (!g) return;

    for (int i = 0; i < g->num_vertices; i++) {
        NoAdjacente *atual = g->lista_adj[i];
        while (atual) {
            NoAdjacente *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }

    for (int i = 0; i < g->capacidade; i++) {
        free(g->matriz_adj[i]);
        free(g->matriz_tipo[i]);
    }

    free(g->matriz_adj);
    free(g->matriz_tipo);
    free(g->lista_adj);
    free(g->vertices);
    free(g);
}

static void garantir_capacidade(Grafo *g) {
    if (g->num_vertices < g->capacidade) return;

    int nova_capacidade = g->capacidade * 2;

    g->vertices = realloc(g->vertices, sizeof(Coordenada) * nova_capacidade);
    g->lista_adj = realloc(g->lista_adj, sizeof(NoAdjacente *) * nova_capacidade);

    for (int i = g->capacidade; i < nova_capacidade; i++) {
        g->lista_adj[i] = NULL;
    }

    int **nova_matriz = malloc(sizeof(int *) * nova_capacidade);
    unsigned char **nova_matriz_tipo = malloc(sizeof(unsigned char *) * nova_capacidade);
    for (int i = 0; i < nova_capacidade; i++) {
        nova_matriz[i] = calloc(nova_capacidade, sizeof(int));
        nova_matriz_tipo[i] = calloc(nova_capacidade, sizeof(unsigned char));
        if (i < g->capacidade) {
            memcpy(nova_matriz[i], g->matriz_adj[i], sizeof(int) * g->capacidade);
            memcpy(nova_matriz_tipo[i], g->matriz_tipo[i], sizeof(unsigned char) * g->capacidade);
            free(g->matriz_adj[i]);
            free(g->matriz_tipo[i]);
        }
    }
    free(g->matriz_adj);
    free(g->matriz_tipo);
    g->matriz_adj = nova_matriz;
    g->matriz_tipo = nova_matriz_tipo;

    g->capacidade = nova_capacidade;
}

int obter_ou_criar_vertice(Grafo *g, double lon, double lat) {
    for (int i = 0; i < g->num_vertices; i++) {
        if (fabs(g->vertices[i].lon - lon) < EPSILON &&
            fabs(g->vertices[i].lat - lat) < EPSILON) {
            return i;
        }
    }

    garantir_capacidade(g);

    int id = g->num_vertices;
    g->vertices[id].lon = lon;
    g->vertices[id].lat = lat;
    g->num_vertices++;

    return id;
}

void adicionar_aresta_tipada(Grafo *g, int origem, int destino, TipoVia tipo) {
    if (origem == destino) return;

    if (g->matriz_adj[origem][destino] == 1) return;

    NoAdjacente *novo1 = malloc(sizeof(NoAdjacente));
    novo1->destino = destino;
    novo1->tipo = tipo;
    novo1->prox = g->lista_adj[origem];
    g->lista_adj[origem] = novo1;

    NoAdjacente *novo2 = malloc(sizeof(NoAdjacente));
    novo2->destino = origem;
    novo2->tipo = tipo;
    novo2->prox = g->lista_adj[destino];
    g->lista_adj[destino] = novo2;

    g->matriz_adj[origem][destino] = 1;
    g->matriz_adj[destino][origem] = 1;
    g->matriz_tipo[origem][destino] = (unsigned char)tipo;
    g->matriz_tipo[destino][origem] = (unsigned char)tipo;

    g->num_arestas++;
}

void adicionar_aresta(Grafo *g, int origem, int destino) {
    adicionar_aresta_tipada(g, origem, destino, VIA_DESCONHECIDA);
}

static char *ler_arquivo_completo(const char *caminho) {
    FILE *f = fopen(caminho, "rb");
    if (!f) {
        fprintf(stderr, "Erro: não foi possível abrir o arquivo '%s'\n", caminho);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(tamanho + 1);
    fread(buffer, 1, tamanho, f);
    buffer[tamanho] = '\0';

    fclose(f);
    return buffer;
}

static TipoVia extrair_tipo_via(const char *inicio, const char *fim) {
    const char *achado = inicio;
    const char *ultimo = NULL;

    while ((achado = strstr(achado, "\"highway\"")) != NULL && achado < fim) {
        ultimo = achado;
        achado++;
    }

    if (!ultimo) return VIA_DESCONHECIDA;

    char texto[64];
    if (sscanf(ultimo, "\"highway\" : \"%63[^\"]\"", texto) != 1) {
        return VIA_DESCONHECIDA;
    }

    return tipo_via_de_texto(texto);
}

Grafo *carregar_grafo_geojson(const char *caminho_arquivo) {
    char *conteudo = ler_arquivo_completo(caminho_arquivo);
    if (!conteudo) {
        return NULL;
    }

    Grafo *g = criar_grafo(512);

    const char *marcador = "\"coordinates\"";
    char *ponteiro = conteudo;
    char *fim_via_anterior = conteudo;

    while ((ponteiro = strstr(ponteiro, marcador)) != NULL) {
        TipoVia tipo_via = extrair_tipo_via(fim_via_anterior, ponteiro);

        ponteiro = strchr(ponteiro, ':');
        if (!ponteiro) break;
        ponteiro++;

        while (*ponteiro == ' ' || *ponteiro == '\n' || *ponteiro == '\r' || *ponteiro == '\t') {
            ponteiro++;
        }
        if (*ponteiro != '[') continue;
        ponteiro++;

        int vertice_anterior = -1;

        while (1) {
            while (*ponteiro == ' ' || *ponteiro == ',' || *ponteiro == '\n' ||
                   *ponteiro == '\r' || *ponteiro == '\t') {
                ponteiro++;
            }

            if (*ponteiro == ']') {
                ponteiro++;
                break;
            }

            if (*ponteiro != '[') {
                break;
            }

            double lon, lat;
            int caracteres_lidos = 0;
            if (sscanf(ponteiro, "[ %lf , %lf ]%n", &lon, &lat, &caracteres_lidos) == 2) {
                ponteiro += caracteres_lidos;

                int vertice_atual = obter_ou_criar_vertice(g, lon, lat);

                if (vertice_anterior != -1) {
                    adicionar_aresta_tipada(g, vertice_anterior, vertice_atual, tipo_via);
                }
                vertice_anterior = vertice_atual;
            } else {
                ponteiro++;
            }
        }

        fim_via_anterior = ponteiro;
    }

    free(conteudo);
    return g;
}

void imprimir_lista_adjacencia(Grafo *g) {
    printf("===== Lista de Adjacencia =====\n");
    for (int i = 0; i < g->num_vertices; i++) {
        printf("%d: ", i);
        NoAdjacente *atual = g->lista_adj[i];
        if (!atual) {
            printf("(sem conexoes)");
        }
        while (atual) {
            printf("%d[%s]", atual->destino, nome_tipo_via(atual->tipo));
            if (atual->prox) printf(" -> ");
            atual = atual->prox;
        }
        printf("\n");
    }
    printf("================================\n");
}

void imprimir_matriz_adjacencia(Grafo *g, int limite) {
    if (limite > g->num_vertices) {
        limite = g->num_vertices;
    }

    printf("===== Matriz de Adjacencia (amostra %dx%d) =====\n", limite, limite);

    printf("     ");
    for (int j = 0; j < limite; j++) {
        printf("%2d ", j);
    }
    printf("\n");

    for (int i = 0; i < limite; i++) {
        printf("%3d: ", i);
        for (int j = 0; j < limite; j++) {
            printf("%2d ", g->matriz_adj[i][j]);
        }
        printf("\n");
    }
    printf("=================================================\n");
}

void imprimir_estatisticas(Grafo *g) {
    printf("===== Estatisticas do Grafo =====\n");
    printf("Vertices (cruzamentos): %d\n", g->num_vertices);
    printf("Arestas (trechos de via): %d\n", g->num_arestas);
    printf("Capacidade alocada: %d\n", g->capacidade);
    printf("==================================\n");
}