#include <stdio.h>
#include "grafo.h"
#include "log.h"

/*
 * Demonstra o uso do sistema de log (issue #5), medindo tempo e
 * memória do carregamento do grafo real de Brumadinho.
 *
 * Qualquer outro algoritmo do projeto (BFS, DFS, Tarjan, Dijkstra...)
 * deve seguir o mesmo padrão: iniciar_cronometro -> rodar -> parar_cronometro
 * -> registrar_log.
 */
int main(int argc, char *argv[]) {
    const char *caminho = "../data/mapa_brumadinho.geojson";
    if (argc > 1) {
        caminho = argv[1];
    }

    Cronometro c;
    iniciar_cronometro(&c);

    Grafo *g = carregar_grafo_geojson(caminho);

    double tempo_ms = parar_cronometro(&c);

    if (!g) {
        fprintf(stderr, "Falha ao carregar o grafo.\n");
        return 1;
    }

    imprimir_estatisticas(g);

    /* Memória "teórica" das estruturas do grafo (lista + matriz),
     * calculada a partir dos tamanhos reais das structs em uso. */
    long memoria_estrutura =
        (long)g->capacidade * sizeof(Coordenada) +                 /* vertices[] */
        (long)g->capacidade * g->capacidade * sizeof(int) +        /* matriz_adj */
        (long)g->num_arestas * 2 * sizeof(NoAdjacente);            /* lista_adj (nós encadeados) */

    registrar_log("Carregamento_Grafo", g->num_vertices, tempo_ms, memoria_estrutura);

    printf("\nEsse resultado também foi salvo em logs/resultados.csv\n");
    printf("(cada execução adiciona uma nova linha, útil para os testes de estresse).\n");

    destruir_grafo(g);
    return 0;
}
