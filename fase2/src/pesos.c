#include <math.h>
#include "pesos.h"

#define RAIO_TERRA_METROS 6371000.0
#define PI 3.14159265358979323846

static double para_radianos(double graus) {
    return graus * PI / 180.0;
}

double distancia_haversine(double lat1, double lon1, double lat2, double lon2) {
    double phi1 = para_radianos(lat1);
    double phi2 = para_radianos(lat2);
    double delta_phi = para_radianos(lat2 - lat1);
    double delta_lambda = para_radianos(lon2 - lon1);

    double seno_phi = sin(delta_phi / 2.0);
    double seno_lambda = sin(delta_lambda / 2.0);

    double a = seno_phi * seno_phi + cos(phi1) * cos(phi2) * seno_lambda * seno_lambda;
    if (a < 0.0) a = 0.0;
    if (a > 1.0) a = 1.0;

    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return RAIO_TERRA_METROS * c;
}

double distancia_entre_vertices(const Grafo *g, int u, int v) {
    return distancia_haversine(g->vertices[u].lat, g->vertices[u].lon,
                               g->vertices[v].lat, g->vertices[v].lon);
}
