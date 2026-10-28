
#include <stdio.h>
#include <math.h>
#include "grafo.h"
#include "pesos.h"

static int falhas = 0;

static void verificar(const char *descricao, int condicao)
{
    printf("[%s] %s\n", condicao ? "OK" : "FALHOU", descricao);

    if (!condicao) {
        falhas++;
    }
}

static void conferir_fator(TipoVia tipo, double esperado)
{
    double obtido = fator_lentidao(tipo);

    int ok = isfinite(obtido) &&
             fabs(obtido - esperado) < 0.001;

    printf("[%s] %-14s fator obtido=%.3f esperado=%.3f\n",
           ok ? "OK" : "FALHOU",
           nome_tipo_via(tipo),
           obtido,
           esperado);

    if (!ok) {
        falhas++;
    }
}

int main(int argc, char *argv[])
{
    const char *caminho = "data/mapa_brumadinho.geojson";

    if (argc > 1) {
        caminho = argv[1];
    }

    imprimir_tabela_lentidao();

    printf("\n===== Valores esperados (40 / velocidade) =====\n");

    conferir_fator(VIA_TERTIARY,      1.0);
    conferir_fator(VIA_TERTIARY_LINK, 40.0 / 30.0);
    conferir_fator(VIA_RESIDENTIAL,   40.0 / 30.0);
    conferir_fator(VIA_UNCLASSIFIED,  40.0 / 30.0);
    conferir_fator(VIA_SERVICE,       2.0);
    conferir_fator(VIA_TRACK,         40.0 / 15.0);
    conferir_fator(VIA_PATH,          8.0);
    conferir_fator(VIA_FOOTWAY,       8.0);
    conferir_fator(VIA_OUTRO,         2.0);
    conferir_fator(VIA_DESCONHECIDA,  2.0);

    /* Categorias adicionais */
    conferir_fator(VIA_PRINCIPAL,     1.0);
    conferir_fator(VIA_SECUNDARIA,    40.0 / 30.0);
    conferir_fator(VIA_LOCAL,         2.0);

    printf("\n===== Propriedades da tabela =====\n");

    int todas_validas = 1;

    for (int t = 0; t < NUM_TIPOS_VIA; t++) {
        double velocidade = velocidade_via_kmh((TipoVia)t);
        double fator = fator_lentidao((TipoVia)t);

        if (velocidade <= 0.0 ||
            !isfinite(velocidade) ||
            fator < 1.0 ||
            !isfinite(fator)) {
            todas_validas = 0;
        }
    }

    verificar("todas as categorias possuem velocidade positiva e fator finito",
              todas_validas);

    verificar("a via de referencia tertiary tem fator exatamente 1",
              fator_lentidao(VIA_TERTIARY) == 1.0);

    verificar("principal tem fator exatamente 1",
              fator_lentidao(VIA_PRINCIPAL) == 1.0);

    verificar("tertiary < unclassified < service < track < path",
              fator_lentidao(VIA_TERTIARY) <
              fator_lentidao(VIA_UNCLASSIFIED) &&
              fator_lentidao(VIA_UNCLASSIFIED) <
              fator_lentidao(VIA_SERVICE) &&
              fator_lentidao(VIA_SERVICE) <
              fator_lentidao(VIA_TRACK) &&
              fator_lentidao(VIA_TRACK) <
              fator_lentidao(VIA_PATH));

    verificar("tipo invalido (99) e tratado como desconhecida",
              fator_lentidao((TipoVia)99) ==
              fator_lentidao(VIA_DESCONHECIDA));

    verificar("tipo negativo e tratado como desconhecido",
              fator_lentidao((TipoVia)-1) ==
              fator_lentidao(VIA_DESCONHECIDA));

    printf("\n===== Impacto na malha de Brumadinho =====\n");

    Grafo *g = carregar_grafo_geojson(caminho);

    if (g == NULL) {
        fprintf(stderr, "Falha ao carregar o grafo.\n");
        return 1;
    }

    double km[NUM_TIPOS_VIA] = {0};
    int arestas[NUM_TIPOS_VIA] = {0};

    for (int i = 0; i < g->num_vertices; i++) {
        for (NoAdjacente *a = g->vertices[i].adjacencias;
             a != NULL;
             a = a->prox) {

            if (i >= a->destino) {
                continue;
            }

            if ((int)a->tipo < 0 || (int)a->tipo >= NUM_TIPOS_VIA) {
                verificar("aresta possui tipo de via valido", 0);
                continue;
            }

            double distancia =
                distancia_entre_vertices(g, i, a->destino);

            if (!isfinite(distancia) || distancia < 0.0) {
                verificar("distancia da aresta e valida", 0);
                continue;
            }

            km[a->tipo] += distancia / 1000.0;
            arestas[a->tipo]++;
        }
    }

    double total_km = 0.0;
    double total_equivalente = 0.0;
    int total_arestas = 0;

    printf("%-15s %8s %9s %7s %12s\n",
           "Tipo da via", "Arestas", "km", "Fator", "km equiv.");
    printf("----------------------------------------------------------\n");

    for (int t = 0; t < NUM_TIPOS_VIA; t++) {
        if (arestas[t] == 0) {
            continue;
        }

        double fator = fator_lentidao((TipoVia)t);
        double equivalente = km[t] * fator;

        printf("%-15s %8d %9.3f %7.2f %12.3f\n",
               nome_tipo_via((TipoVia)t),
               arestas[t],
               km[t],
               fator,
               equivalente);

        total_km += km[t];
        total_equivalente += equivalente;
        total_arestas += arestas[t];
    }

    printf("----------------------------------------------------------\n");
    printf("%-15s %8d %9.3f %7s %12.3f\n",
           "TOTAL",
           total_arestas,
           total_km,
           "",
           total_equivalente);

    if (total_km > 0.0) {
        printf("\nMalha efetiva / malha real: %.3f\n",
               total_equivalente / total_km);
    } else {
        verificar("comprimento total da malha e positivo", 0);
    }

    verificar("total de arestas confere com o grafo",
              total_arestas == g->num_arestas);

    printf("\n%s\n",
           falhas == 0 ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHAS");

    destruir_grafo(g);

    return falhas == 0 ? 0 : 1;
}
