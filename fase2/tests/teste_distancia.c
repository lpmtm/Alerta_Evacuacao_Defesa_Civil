#include <stdio.h>
#include <math.h>
#include "grafo.h"
#include "pesos.h"

static int falhas = 0;

static void conferir(const char *descricao, double obtido, double esperado, double tolerancia) {
    int ok = fabs(obtido - esperado) <= tolerancia;
    printf("[%s] %s: obtido=%.2f m, esperado=%.2f m\n", ok ? "OK" : "FALHOU", descricao, obtido, esperado);
    if (!ok) falhas++;
}

int main(int argc, char *argv[]) {
    const char *caminho = "data/mapa_brumadinho.geojson";
    if (argc > 1) {
        caminho = argv[1];
    }

    printf("===== Casos de referencia =====\n");
    conferir("mesmo ponto", distancia_haversine(-20.1197, -44.1214, -20.1197, -44.1214), 0.0, 0.001);
    conferir("1 grau de latitude no equador", distancia_haversine(0.0, 0.0, 1.0, 0.0), 111194.93, 1.0);
    conferir("1 grau de longitude no equador", distancia_haversine(0.0, 0.0, 0.0, 1.0), 111194.93, 1.0);
    conferir("Paris -> Londres (coerencia com a lei dos cossenos)", distancia_haversine(48.8566, 2.3522, 51.5074, -0.1278), 343556.06, 5.0);
    conferir("ida e volta (simetria)",
             distancia_haversine(-20.1197, -44.1214, -20.1377, -44.1082),
             distancia_haversine(-20.1377, -44.1082, -20.1197, -44.1214), 0.000001);

    printf("\n===== Grafo de Brumadinho =====\n");
    Grafo *g = carregar_grafo_geojson(caminho);
    if (!g) {
        fprintf(stderr, "Falha ao carregar o grafo.\n");
        return 1;
    }

    int arestas = 0;
    int invalidas = 0;
    double soma = 0.0;
    double menor = 0.0;
    double maior = 0.0;

    for (int i = 0; i < g->num_vertices; i++) {
        for (NoAdjacente *a = g->lista_adj[i]; a; a = a->prox) {
            if (i >= a->destino) continue;

            double d = distancia_entre_vertices(g, i, a->destino);
            if (isnan(d) || d <= 0.0) {
                invalidas++;
                continue;
            }

            if (arestas == 0 || d < menor) menor = d;
            if (arestas == 0 || d > maior) maior = d;
            soma += d;
            arestas++;
        }
    }

    printf("Arestas medidas: %d\n", arestas);
    printf("Comprimento total das vias: %.3f km\n", soma / 1000.0);
    printf("Menor trecho: %.2f m\n", menor);
    printf("Maior trecho: %.2f m\n", maior);
    printf("Trecho médio: %.2f m\n", arestas ? soma / arestas : 0.0);
    printf("Arestas com distância invalida (zero ou NaN): %d\n", invalidas);

    if (invalidas > 0) falhas++;

    printf("\n%s\n", falhas == 0 ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHAS");

    destruir_grafo(g);
    return falhas == 0 ? 0 : 1;
}
