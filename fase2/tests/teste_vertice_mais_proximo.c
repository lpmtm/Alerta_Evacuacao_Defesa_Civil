#include <stdio.h>
#include "grafo.h"
#include "pesos.h"
#include "geo.h"

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

    printf("===== Casos de borda =====\n");
    conferir("grafo NULL devolve -1", vertice_mais_proximo(NULL, BARRAGEM_LAT, BARRAGEM_LON) == -1);

    Grafo *vazio = criar_grafo(1);
    conferir("grafo sem vertices devolve -1", vertice_mais_proximo(vazio, BARRAGEM_LAT, BARRAGEM_LON) == -1);
    destruir_grafo(vazio);

    Grafo *pequeno = criar_grafo(4);
    int a = obter_ou_criar_vertice(pequeno, -44.0000, -20.0000);
    int b = obter_ou_criar_vertice(pequeno, -44.0100, -20.0000);
    int c = obter_ou_criar_vertice(pequeno, -44.0000, -20.0100);
    conferir("ponto exato de um vertice devolve ele mesmo", vertice_mais_proximo(pequeno, -20.0100, -44.0000) == c);
    conferir("ponto perto de A devolve A", vertice_mais_proximo(pequeno, -20.0010, -44.0010) == a);
    conferir("ponto perto de B devolve B", vertice_mais_proximo(pequeno, -20.0010, -44.0090) == b);
    destruir_grafo(pequeno);

    printf("\n===== Grafo de Brumadinho =====\n");
    Grafo *g = carregar_grafo_geojson(caminho);
    if (!g) {
        fprintf(stderr, "Falha ao carregar o grafo.\n");
        return 1;
    }

    int errados = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        int v = vertice_mais_proximo(g, g->vertices[i].lat, g->vertices[i].lon);
        if (distancia_entre_vertices(g, i, v) > 0.0) errados++;
    }
    printf("Vertices testados: %d, que nao acharam a si mesmos: %d\n", g->num_vertices, errados);
    conferir("cada vertice e o mais proximo da propria coordenada", errados == 0);

    int origem = vertice_mais_proximo(g, BARRAGEM_LAT, BARRAGEM_LON);
    conferir("barragem tem um vertice mais proximo valido", origem >= 0 && origem < g->num_vertices);
    if (origem >= 0) {
        double d = distancia_haversine(BARRAGEM_LAT, BARRAGEM_LON,
                                       g->vertices[origem].lat, g->vertices[origem].lon);
        printf("Barragem (%.4f, %.4f) -> vertice %d em (%.6f, %.6f), a %.2f m\n",
               BARRAGEM_LAT, BARRAGEM_LON, origem,
               g->vertices[origem].lat, g->vertices[origem].lon, d);
    }

    printf("\n%s\n", falhas == 0 ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHAS");

    destruir_grafo(g);
    return falhas == 0 ? 0 : 1;
}
