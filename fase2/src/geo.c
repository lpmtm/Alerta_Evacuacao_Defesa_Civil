#include "geo.h"
#include "pesos.h"

int vertice_mais_proximo(Grafo *g, double lat, double lon) {
    if (!g || g->num_vertices <= 0) {
        return -1;
    }

    int mais_proximo = 0;
    double menor = distancia_haversine(lat, lon, g->vertices[0].lat, g->vertices[0].lon);

    for (int i = 1; i < g->num_vertices; i++) {
        double d = distancia_haversine(lat, lon, g->vertices[i].lat, g->vertices[i].lon);
        if (d < menor) {
            menor = d;
            mais_proximo = i;
        }
    }

    return mais_proximo;
}
