#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "grafo.h" 

// --- PASSO 4: FUNÇÃO PARA EXPORTAR O SUBGRAFO INDUZIDO ---
void exportar_subgrafo_induzido(Grafo* grafo, bool* selecionados, const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo %s\n", nome_arquivo);
        return;
    }

    fprintf(arquivo, "origem,destino\n"); // Cabeçalho CSV

    // Percorre todos os vértices do grafo
    for (int u = 0; u < grafo->num_vertices; u++) {
        if (selecionados[u]) {
            NoAdjacente* vizinho = grafo->lista_adj[u]; 
            
            while (vizinho != NULL) {
                int v = vizinho->destino;
                
                // Salva a aresta se o destino também foi selecionado na BFS.
                // A condição (u < v) evita duplicar a aresta, já que o grafo é não-direcionado.
                if (selecionados[v] && u < v) {
                    fprintf(arquivo, "%d,%d\n", u, v); 
                }
                vizinho = vizinho->prox;
            }
        }
    }

    fclose(arquivo);
    printf("Arquivo gerado com sucesso: %s\n", nome_arquivo);
}

// --- PASSO 3: LÓGICA DE EXTRAÇÃO (BFS) ---
void gerar_subgrafo_conexo(Grafo* grafo, int vertice_raiz, int N, const char* nome_arquivo) {
    int total_vertices = grafo->num_vertices;
    
    // Evita tentar extrair mais vértices do que o grafo possui
    if (N > total_vertices) N = total_vertices;

    bool* visitados = (bool*)calloc(total_vertices, sizeof(bool));
    bool* selecionados = (bool*)calloc(total_vertices, sizeof(bool)); 
    int* fila = (int*)malloc(total_vertices * sizeof(int));
    
    int inicio = 0, fim = 0;
    int contador_selecionados = 0;

    // Inicia a BFS
    fila[fim++] = vertice_raiz;
    visitados[vertice_raiz] = true;

    while (inicio < fim && contador_selecionados < N) {
        int u = fila[inicio++]; 
        
        selecionados[u] = true;
        contador_selecionados++;

        if (contador_selecionados == N) {
            break;
        }

        NoAdjacente* vizinho = grafo->lista_adj[u];
        while (vizinho != NULL) {
            int v = vizinho->destino;
            if (!visitados[v]) {
                visitados[v] = true;
                fila[fim++] = v;
            }
            vizinho = vizinho->prox;
        }
    }

    exportar_subgrafo_induzido(grafo, selecionados, nome_arquivo);

    free(visitados);
    free(selecionados);
    free(fila);
}

int main() {
    // 1. Carrega o mapa base usando a função da sua equipe
    printf("Carregando o grafo completo...\n");
    Grafo* mapa_completo = carregar_grafo_geojson("fase1/data/mapa_brumadinho.geojson"); 
    
    if (mapa_completo == NULL) {
        printf("Erro ao carregar o arquivo geojson.\n");
        return 1;
    }

    // 2. Vértice de partida (o ID 0 geralmente é um bom ponto de partida)
    int raiz = 0; 

    // 3. Gera os 3 arquivos de teste em formato CSV
    printf("Iniciando a extracao dos subgrafos...\n");
    gerar_subgrafo_conexo(mapa_completo, raiz, 100, "fase1/data/subgrafo_100.csv");
    gerar_subgrafo_conexo(mapa_completo, raiz, 500, "fase1/data/subgrafo_500.csv");
    gerar_subgrafo_conexo(mapa_completo, raiz, 1000, "fase1/data/subgrafo_1000.csv");

    // Libera a memória usando a função da sua equipe
    destruir_grafo(mapa_completo);
    
    return 0;
}