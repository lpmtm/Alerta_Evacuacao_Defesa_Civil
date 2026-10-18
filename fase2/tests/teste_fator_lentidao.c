#include <stdio.h>
#include <math.h>
#include "grafo.h"
#include "pesos.h"

static int falhas = 0;

static void verificar(const char *descricao, int condicao) {
    printf("[%s] %s\n", condicao ? "OK" : "FALHOU", descricao);
    if (!condicao) falhas++;
}

static void conferir_fator(TipoVia tipo, double esperado) {
    double obtido = fator_lentidao(tipo);
    int ok = fabs(obtido - esperado) < 0.001;
    printf("[%s] %-14s fator obtido=%.3f esperado=%.3f\n", ok ? "OK" : "FALHOU",
           nome_tipo_via(tipo), obtido, esperado);
    if (!ok) falhas++;
}

int main(int argc, char *argv[]) {
    const char *caminho = "data/mapa_brumadinho.geojson";
    if (argc > 1) {
        caminho = argv[1];
    }

    imprimir_tabela_lentidao();

    printf("\n===== Valores esperados (calculados a mao: 40 / velocidade) =====\n");
    conferir_fator(VIA_TERTIARY, 1.0);
    conferir_fator(VIA_TERTIARY_LINK, 40.0 / 30.0);
    conferir_fator(VIA_RESIDENTIAL, 40.0 / 30.0);
    conferir_fator(VIA_UNCLASSIFIED, 40.0 / 30.0);
    conferir_fator(VIA_SERVICE, 2.0);
    conferir_fator(VIA_TRACK, 40.0 / 15.0);
    conferir_fator(VIA_PATH, 8.0);
    conferir_fator(VIA_FOOTWAY, 8.0);
    conferir_fator(VIA_OUTRO, 2.0);
    conferir_fator(VIA_DESCONHECIDA, 2.0);

    printf("\n===== Propriedades que a tabela precisa respeitar =====\n");
    int todos_maiores_que_um = 1;
    for (int t = 0; t < NUM_TIPOS_VIA; t++) {
        if (fator_lentidao((TipoVia)t) < 1.0) todos_maiores_que_um = 0;
    }
    verificar("nenhum fator é menor que 1 (a via de referência é a mais rápida)", todos_maiores_que_um);
    verificar("a via de referência (tertiary) tem fator exatamente 1",
              fator_lentidao(VIA_TERTIARY) == 1.0);
    verificar("tertiary < unclassified < service < track < path",
              fator_lentidao(VIA_TERTIARY) < fator_lentidao(VIA_UNCLASSIFIED) &&
              fator_lentidao(VIA_UNCLASSIFIED) < fator_lentidao(VIA_SERVICE) &&
              fator_lentidao(VIA_SERVICE) < fator_lentidao(VIA_TRACK) &&
              fator_lentidao(VIA_TRACK) < fator_lentidao(VIA_PATH));
    verificar("tipo inválido (99) é tratado como desconhecida",
              fator_lentidao((TipoVia)99) == fator_lentidao(VIA_DESCONHECIDA));

    printf("\n===== Impacto na malha de Brumadinho =====\n");
    Grafo *g = carregar_grafo_geojson(caminho);
    if (!g) {
        fprintf(stderr, "Falha ao carregar o grafo.\n");
        return 1;
    }

    double km[NUM_TIPOS_VIA] = {0};
    int arestas[NUM_TIPOS_VIA] = {0};
    for (int i = 0; i < g->num_vertices; i++) {
        for (NoAdjacente *a = g->lista_adj[i]; a; a = a->prox) {
            if (i >= a->destino) continue;
            km[a->tipo] += distancia_entre_vertices(g, i, a->destino) / 1000.0;
            arestas[a->tipo]++;
        }
    }

    double total_km = 0.0;
    double total_equivalente = 0.0;
    int total_arestas = 0;

    printf("%-15s %8s %9s %7s %12s\n", "Tipo da via", "Arestas", "km", "Fator", "km equiv.");
    printf("----------------------------------------------------------\n");
    for (int t = 0; t < NUM_TIPOS_VIA; t++) {
        if (arestas[t] == 0) continue;
        double equivalente = km[t] * fator_lentidao((TipoVia)t);
        printf("%-15s %8d %9.3f %7.2f %12.3f\n", nome_tipo_via((TipoVia)t),
               arestas[t], km[t], fator_lentidao((TipoVia)t), equivalente);
        total_km += km[t];
        total_equivalente += equivalente;
        total_arestas += arestas[t];
    }
    printf("----------------------------------------------------------\n");
    printf("%-15s %8d %9.3f %7s %12.3f\n", "TOTAL", total_arestas, total_km, "", total_equivalente);
    printf("\nMalha efetiva / malha real: %.3f\n", total_equivalente / total_km);

    verificar("total de arestas confere com o grafo", total_arestas == g->num_arestas);

    printf("\n%s\n", falhas == 0 ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHAS");

    destruir_grafo(g);
    return falhas == 0 ? 0 : 1;
}
