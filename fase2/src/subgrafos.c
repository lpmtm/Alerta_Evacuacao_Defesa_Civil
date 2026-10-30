#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "subgrafos.h"
#include "grafo.h"
#include "log.h"

void gerar_subgrafo_conexo(Grafo *g_origem, int n_vertices, int vertice_inicial, const char *ficheiro_saida) {
    if (!g_origem || g_origem->quantidade_vertices <= 0) {
        fprintf(stderr, "Erro: Grafo invalido ou vazio.\n");
        return;
    }

    int total_vertices = g_origem->quantidade_vertices;

    if (n_vertices > total_vertices) {
        fprintf(stderr, "Erro: N (%d) eh maior que o total de vertices (%d).\n", n_vertices, total_vertices);
        return;
    }

    bool *visitado = (bool *) log_calloc((size_t)total_vertices, sizeof(bool));
    int *vertices_sub = (int *) log_malloc((size_t)n_vertices * sizeof(int));
    int count = 0;

    // Fila para a travessia BFS
    int *fila = (int *) log_malloc((size_t)total_vertices * sizeof(int));
    int inicio = 0, fim = 0;

    // Iniciar a BFS no vértice inicial
    visitado[vertice_inicial] = true;
    fila[fim++] = vertice_inicial;
    vertices_sub[count++] = vertice_inicial;

    while (inicio < fim && count < n_vertices) {
        int u = fila[inicio++];

        // Percorre a lista de adjacências de 'u'
        for (NoAdjacente *a = g_origem->vertices[u].adjacencias; a != NULL; a = a->prox) {
            int v = a->destino;
            if (!visitado[v]) {
                visitado[v] = true;
                fila[fim++] = v;
                vertices_sub[count++] = v;

                if (count == n_vertices) {
                    break;
                }
            }
        }
    }

    log_free(fila);
    log_free(visitado);

    if (count < n_vertices) {
        fprintf(stderr, "Aviso: A componente conexa possui apenas %d vertices (menos que N=%d).\n", count, n_vertices);
    }

    // Garante a existência da pasta de saída antes de gravar
    #if defined(_WIN32)
        system("if not exist saida mkdir saida");
    #else
        system("mkdir -p saida");
    #endif

    FILE *f = fopen(ficheiro_saida, "w");
    if (!f) {
        perror("Erro ao abrir ficheiro para gravar o subgrafo");
        log_free(vertices_sub);
        return;
    }

    fprintf(f, "# Subgrafo induzido extraido via BFS (N=%d)\n", count);
    fprintf(f, "VERTICES %d\n", count);
    for (int i = 0; i < count; i++) {
        int orig_id = vertices_sub[i];
        fprintf(f, "%d %f %f\n", i, g_origem->vertices[orig_id].lat, g_origem->vertices[orig_id].lon);
    }

    // Gravando as arestas induzidas mantendo os índices de 0 a count-1
    fprintf(f, "ARESTAS\n");
    int num_arestas_sub = 0;
    
    // Mapeamento inverso para busca rápida em O(1)
    int *mapa_novo_idx = (int *) log_malloc((size_t)total_vertices * sizeof(int));
    for (int i = 0; i < total_vertices; i++) mapa_novo_idx[i] = -1;
    for (int i = 0; i < count; i++) mapa_novo_idx[vertices_sub[i]] = i;

    for (int i = 0; i < count; i++) {
        int u_orig = vertices_sub[i];
        for (NoAdjacente *a = g_origem->vertices[u_orig].adjacencias; a != NULL; a = a->prox) {
            int v_orig = a->destino;
            int v_novo = mapa_novo_idx[v_orig];
            
            // Grava apenas uma vez para grafos não direcionados (u_novo < v_novo)
            if (v_novo != -1 && i < v_novo) {
                fprintf(f, "%d %d %s\n", i, v_novo, nome_tipo_via(a->tipo));
                num_arestas_sub++;
            }
        }
    }

    fclose(f);
    log_free(mapa_novo_idx);
    log_free(vertices_sub);

    printf("[P4] Subgrafo conexo com N=%d (%d arestas) gerado com sucesso em '%s'.\n", 
           count, num_arestas_sub, ficheiro_saida);
}