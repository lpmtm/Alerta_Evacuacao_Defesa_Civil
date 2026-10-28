
#include <stdio.h>
#include <math.h>
#include "pesos.h"

#define RAIO_TERRA_METROS 6371000.0
#define PI 3.14159265358979323846
#define VELOCIDADE_REFERENCIA_KMH 40.0

static const double VELOCIDADE_VIA_KMH[NUM_TIPOS_VIA] = {
    [VIA_DESCONHECIDA]  = 20.0,
    [VIA_SERVICE]       = 20.0,
    [VIA_TRACK]         = 15.0,
    [VIA_TERTIARY]      = 40.0,
    [VIA_TERTIARY_LINK] = 30.0,
    [VIA_UNCLASSIFIED]  = 30.0,
    [VIA_RESIDENTIAL]   = 30.0,
    [VIA_PATH]          = 5.0,
    [VIA_FOOTWAY]       = 5.0,
    [VIA_OUTRO]         = 20.0,

    /* Categorias adicionais */
    [VIA_PRINCIPAL]     = 40.0,
    [VIA_SECUNDARIA]    = 30.0,
    [VIA_LOCAL]         = 20.0
};

static double para_radianos(double graus)
{
    return graus * PI / 180.0;
}

double distancia_haversine(double lat1, double lon1,
                           double lat2, double lon2)
{
    double phi1 = para_radianos(lat1);
    double phi2 = para_radianos(lat2);
    double delta_phi = para_radianos(lat2 - lat1);
    double delta_lambda = para_radianos(lon2 - lon1);

    double seno_phi = sin(delta_phi / 2.0);
    double seno_lambda = sin(delta_lambda / 2.0);

    double a = seno_phi * seno_phi
             + cos(phi1) * cos(phi2)
             * seno_lambda * seno_lambda;

    if (a < 0.0) {
        a = 0.0;
    }

    if (a > 1.0) {
        a = 1.0;
    }

    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return RAIO_TERRA_METROS * c;
}

double distancia_entre_vertices(const Grafo *g, int u, int v)
{
    if (g == NULL ||
        g->vertices == NULL ||
        u < 0 || u >= g->num_vertices ||
        v < 0 || v >= g->num_vertices) {
        return NAN;
    }

    return distancia_haversine(
        g->vertices[u].lat,
        g->vertices[u].lon,
        g->vertices[v].lat,
        g->vertices[v].lon
    );
}

double velocidade_via_kmh(TipoVia tipo)
{
    if ((int)tipo < 0 || (int)tipo >= NUM_TIPOS_VIA) {
        tipo = VIA_DESCONHECIDA;
    }

    return VELOCIDADE_VIA_KMH[tipo];
}

double fator_lentidao(TipoVia tipo)
{
    double velocidade = velocidade_via_kmh(tipo);

    if (velocidade <= 0.0 || !isfinite(velocidade)) {
        return INFINITY;
    }

    return VELOCIDADE_REFERENCIA_KMH / velocidade;
}

void imprimir_tabela_lentidao(void)
{
    printf("===== Tabela de lentidao por tipo de via =====\n");
    printf("Velocidade de referencia: %.0f km/h (fator 1.00)\n\n",
           VELOCIDADE_REFERENCIA_KMH);

    printf("%-15s %12s %8s\n",
           "Tipo da via", "Velocidade", "Fator");
    printf("----------------------------------------\n");

    for (int t = 0; t < NUM_TIPOS_VIA; t++) {
        printf("%-15s %9.0f km/h %8.2f\n",
               nome_tipo_via((TipoVia)t),
               velocidade_via_kmh((TipoVia)t),
               fator_lentidao((TipoVia)t));
    }

    printf("==============================================\n");
}
