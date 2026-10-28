#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "geo.h"
#include "propagacao.h"

static int falhas = 0;

static void conferir(const char *descricao, int ok) {
    printf("[%s] %s\n", ok ? "OK" : "FALHOU", descricao);
    if (!ok) falhas++;
}

int main(int argc, char *argv[]) {
    const char *caminho = "data/mapa_brumadinho.geojson";
    if (argc > 1) {
        caminho = argv[1];
    }

    printf("===== Grafo pequeno =====\n");
    /* 0 - 1 - 2 - 3, 0 - 4 - 3 e o vertice 5 isolado */
    Grafo *p = criar_grafo(6);
    for (int i = 0; i < 6; i++) {
        obter_ou_criar_vertice(p, -44.0 - i * 0.001, -20.0);
    }
    adicionar_aresta(p, 0, 1);
    adicionar_aresta(p, 1, 2);
    adicionar_aresta(p, 2, 3);
    adicionar_aresta(p, 0, 4);
    adicionar_aresta(p, 4, 3);

    int camada_p[6];
    conferir("origem invalida devolve 0", simular_propagacao(p, 6, camada_p) == 0);
    int atingidos_p = simular_propagacao(p, 0, camada_p);
    conferir("5 vertices atingidos", atingidos_p == 5);
    conferir("origem na camada 0", camada_p[0] == 0);
    conferir("vizinhos diretos na camada 1", camada_p[1] == 1 && camada_p[4] == 1);
    conferir("vertice 2 na camada 2", camada_p[2] == 2);
    conferir("vertice 3 pelo caminho mais curto (camada 2)", camada_p[3] == 2);
    conferir("vertice isolado nao e atingido", camada_p[5] == CAMADA_NAO_ATINGIDO);
    destruir_grafo(p);

    printf("\n===== Grafo de Brumadinho =====\n");
    Grafo *g = carregar_grafo_geojson(caminho);
    if (!g) {
        fprintf(stderr, "Falha ao carregar o grafo.\n");
        return 1;
    }

    int origem = vertice_mais_proximo(g, BARRAGEM_LAT, BARRAGEM_LON);
    int *camada = malloc(sizeof(int) * g->num_vertices);
    int atingidos = simular_propagacao(g, origem, camada);

    int contados = 0;
    int salto_maior_que_um = 0;
    int vizinho_nao_atingido = 0;
    int sem_pai = 0;

    for (int u = 0; u < g->num_vertices; u++) {
        if (camada[u] == CAMADA_NAO_ATINGIDO) continue;
        contados++;

        int tem_pai = (u == origem);
        for (NoAdjacente *a = g->vertices[u].adjacencias; a; a = a->prox) {
            int v = a->destino;
            if (camada[v] == CAMADA_NAO_ATINGIDO) {
                vizinho_nao_atingido++;
                continue;
            }
            int diferenca = camada[u] - camada[v];
            if (diferenca > 1 || diferenca < -1) salto_maior_que_um++;
            if (diferenca == 1) tem_pai = 1;
        }
        if (!tem_pai) sem_pai++;
    }

    printf("Origem: vertice %d | atingidos: %d de %d\n", origem, atingidos, g->num_vertices);
    conferir("origem na camada 0", camada[origem] == 0);
    conferir("retorno bate com os vertices marcados", atingidos == contados);
    conferir("nenhuma aresta liga camadas com diferenca maior que 1", salto_maior_que_um == 0);
    conferir("todo vizinho de um vertice atingido tambem e atingido", vizinho_nao_atingido == 0);
    conferir("todo atingido (menos a origem) tem vizinho na camada anterior", sem_pai == 0);

    printf("\n%s\n", falhas == 0 ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHAS");

    free(camada);
    destruir_grafo(g);
    return falhas == 0 ? 0 : 1;
}
