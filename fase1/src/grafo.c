#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "grafo.h"

/* Tolerância para comparar duas coordenadas como "o mesmo ponto".
 * Coordenadas geográficas vindas do OSM raramente repetem o valor
 * EXATO em ponto flutuante, mas para o mesmo cruzamento o valor
 * costuma ser idêntico no arquivo, então uma tolerância pequena basta. */
#define EPSILON 0.0000001

/* ---------------------------------------------------------- */
/* Criação e destruição                                        */
/* ---------------------------------------------------------- */

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

    /* Matriz de adjacência: aloca capacidade_inicial x capacidade_inicial,
     * inicializada com zeros (calloc já zera a memória). */
    g->matriz_adj = malloc(sizeof(int *) * capacidade_inicial);
    for (int i = 0; i < capacidade_inicial; i++) {
        g->matriz_adj[i] = calloc(capacidade_inicial, sizeof(int));
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
    }

    free(g->matriz_adj);
    free(g->lista_adj);
    free(g->vertices);
    free(g);
}

/* ---------------------------------------------------------- */
/* Vértices e arestas                                          */
/* ---------------------------------------------------------- */

/* Aumenta a capacidade do grafo quando necessário (dobra o tamanho) */
static void garantir_capacidade(Grafo *g) {
    if (g->num_vertices < g->capacidade) return;

    int nova_capacidade = g->capacidade * 2;

    g->vertices = realloc(g->vertices, sizeof(Coordenada) * nova_capacidade);
    g->lista_adj = realloc(g->lista_adj, sizeof(NoAdjacente *) * nova_capacidade);

    /* zera a parte nova da lista de adjacência */
    for (int i = g->capacidade; i < nova_capacidade; i++) {
        g->lista_adj[i] = NULL;
    }

    /* Recria a matriz de adjacência com o novo tamanho */
    int **nova_matriz = malloc(sizeof(int *) * nova_capacidade);
    for (int i = 0; i < nova_capacidade; i++) {
        nova_matriz[i] = calloc(nova_capacidade, sizeof(int));
        if (i < g->capacidade) {
            /* copia os valores antigos */
            memcpy(nova_matriz[i], g->matriz_adj[i], sizeof(int) * g->capacidade);
            free(g->matriz_adj[i]);
        }
    }
    free(g->matriz_adj);
    g->matriz_adj = nova_matriz;

    g->capacidade = nova_capacidade;
}

int obter_ou_criar_vertice(Grafo *g, double lon, double lat) {
    /* Procura se a coordenada já existe.
     * OBS: busca linear O(n). Para os tamanhos deste projeto (milhares
     * de vértices) é aceitável nesta fase; pode ser otimizado depois
     * com uma tabela hash, se necessário. */
    for (int i = 0; i < g->num_vertices; i++) {
        if (fabs(g->vertices[i].lon - lon) < EPSILON &&
            fabs(g->vertices[i].lat - lat) < EPSILON) {
            return i;
        }
    }

    /* Não encontrou: cria um novo vértice */
    garantir_capacidade(g);

    int id = g->num_vertices;
    g->vertices[id].lon = lon;
    g->vertices[id].lat = lat;
    g->num_vertices++;

    return id;
}

void adicionar_aresta(Grafo *g, int origem, int destino) {
    if (origem == destino) return; /* evita laço (self-loop) */

    /* Verifica se a aresta já existe na matriz, para não duplicar */
    if (g->matriz_adj[origem][destino] == 1) return;

    /* Lista de adjacência (grafo não-direcionado: adiciona nos dois sentidos) */
    NoAdjacente *novo1 = malloc(sizeof(NoAdjacente));
    novo1->destino = destino;
    novo1->prox = g->lista_adj[origem];
    g->lista_adj[origem] = novo1;

    NoAdjacente *novo2 = malloc(sizeof(NoAdjacente));
    novo2->destino = origem;
    novo2->prox = g->lista_adj[destino];
    g->lista_adj[destino] = novo2;

    /* Matriz de adjacência */
    g->matriz_adj[origem][destino] = 1;
    g->matriz_adj[destino][origem] = 1;

    g->num_arestas++;
}

/* ---------------------------------------------------------- */
/* Parser simples de GeoJSON (somente o necessário)             */
/* ---------------------------------------------------------- */

/*
 * Este NÃO é um parser de JSON genérico -- é um parser bem específico
 * para o formato exportado pelo Overpass Turbo, que procura blocos
 * "coordinates": [ [lon, lat], [lon, lat], ... ]
 * e extrai os pares de números.
 *
 * Isso evita depender de bibliotecas externas de JSON só para ler
 * um arquivo com um formato simples e previsível.
 */

/* Lê o arquivo inteiro para a memória e retorna um buffer de texto */
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

Grafo *carregar_grafo_geojson(const char *caminho_arquivo) {
    char *conteudo = ler_arquivo_completo(caminho_arquivo);
    if (!conteudo) {
        return NULL;
    }

    /* Começa com capacidade para 512 vértices; cresce sozinho se precisar */
    Grafo *g = criar_grafo(512);

    const char *marcador = "\"coordinates\"";
    char *ponteiro = conteudo;

    while ((ponteiro = strstr(ponteiro, marcador)) != NULL) {
        /* avança até o ':' depois de "coordinates" */
        ponteiro = strchr(ponteiro, ':');
        if (!ponteiro) break;
        ponteiro++; /* pula o ':' */

        /* pula espaços em branco até o '[' que abre o array externo */
        while (*ponteiro == ' ' || *ponteiro == '\n' || *ponteiro == '\r' || *ponteiro == '\t') {
            ponteiro++;
        }
        if (*ponteiro != '[') continue; /* formato inesperado, ignora */
        ponteiro++; /* entra no array externo */

        int vertice_anterior = -1; /* para ligar pontos consecutivos da mesma via */

        while (1) {
            /* pula espaços, vírgulas e quebras de linha */
            while (*ponteiro == ' ' || *ponteiro == ',' || *ponteiro == '\n' ||
                   *ponteiro == '\r' || *ponteiro == '\t') {
                ponteiro++;
            }

            if (*ponteiro == ']') {
                /* fim do array externo (fim desta via) */
                ponteiro++;
                break;
            }

            if (*ponteiro != '[') {
                /* algo inesperado: aborta esta via com segurança */
                break;
            }

            /* lê o par [lon, lat] */
            double lon, lat;
            int caracteres_lidos = 0;
            if (sscanf(ponteiro, "[ %lf , %lf ]%n", &lon, &lat, &caracteres_lidos) == 2) {
                ponteiro += caracteres_lidos;

                int vertice_atual = obter_ou_criar_vertice(g, lon, lat);

                if (vertice_anterior != -1) {
                    adicionar_aresta(g, vertice_anterior, vertice_atual);
                }
                vertice_anterior = vertice_atual;
            } else {
                /* não conseguiu ler o par: avança um caractere para não travar */
                ponteiro++;
            }
        }
    }

    free(conteudo);
    return g;
}

/* ---------------------------------------------------------- */
/* Utilidades                                                   */
/* ---------------------------------------------------------- */

void imprimir_estatisticas(Grafo *g) {
    printf("===== Estatísticas do Grafo =====\n");
    printf("Vértices (cruzamentos): %d\n", g->num_vertices);
    printf("Arestas (trechos de via): %d\n", g->num_arestas);
    printf("Capacidade alocada: %d\n", g->capacidade);
    printf("==================================\n");
}
