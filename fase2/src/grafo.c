#include "grafo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define TOLERANCIA_COORDENADA 0.0000001

static char *duplicar_texto(const char *inicio, size_t tamanho)
{
    char *resultado = malloc(tamanho + 1);

    if (!resultado) {
        return NULL;
    }

    memcpy(resultado, inicio, tamanho);
    resultado[tamanho] = '\0';

    return resultado;
}

static const char *pular_espacos(const char *p, const char *fim)
{
    while (p < fim && isspace((unsigned char)*p)) {
        p++;
    }

    return p;
}

static const char *fim_objeto(const char *inicio, const char *limite)
{
    int profundidade = 0;
    int dentro_string = 0;
    int escape = 0;

    for (const char *p = inicio; p < limite; p++) {
        char c = *p;

        if (dentro_string) {
            if (escape) {
                escape = 0;
            } else if (c == '\\') {
                escape = 1;
            } else if (c == '"') {
                dentro_string = 0;
            }

            continue;
        }

        if (c == '"') {
            dentro_string = 1;
        } else if (c == '{') {
            profundidade++;
        } else if (c == '}') {
            profundidade--;

            if (profundidade == 0) {
                return p + 1;
            }
        }
    }

    return NULL;
}

static const char *fim_array(const char *inicio, const char *limite)
{
    int profundidade = 0;
    int dentro_string = 0;
    int escape = 0;

    for (const char *p = inicio; p < limite; p++) {
        char c = *p;

        if (dentro_string) {
            if (escape) {
                escape = 0;
            } else if (c == '\\') {
                escape = 1;
            } else if (c == '"') {
                dentro_string = 0;
            }

            continue;
        }

        if (c == '"') {
            dentro_string = 1;
        } else if (c == '[') {
            profundidade++;
        } else if (c == ']') {
            profundidade--;

            if (profundidade == 0) {
                return p + 1;
            }
        }
    }

    return NULL;
}

static char *extrair_string(
    const char *inicio,
    const char *fim,
    const char *chave
)
{
    char busca[128];

    if (snprintf(busca, sizeof(busca), "\"%s\"", chave)
        >= (int)sizeof(busca)) {
        return NULL;
    }

    const char *p = inicio;

    while (p < fim) {
        const char *encontrado = strstr(p, busca);

        if (!encontrado || encontrado >= fim) {
            return NULL;
        }

        const char *valor = encontrado + strlen(busca);
        valor = pular_espacos(valor, fim);

        if (valor >= fim || *valor != ':') {
            p = encontrado + strlen(busca);
            continue;
        }

        valor++;
        valor = pular_espacos(valor, fim);

        if (valor >= fim || *valor != '"') {
            return NULL;
        }

        valor++;

        const char *final = valor;
        int escape = 0;

        while (final < fim) {
            if (escape) {
                escape = 0;
            } else if (*final == '\\') {
                escape = 1;
            } else if (*final == '"') {
                break;
            }

            final++;
        }

        if (final >= fim) {
            return NULL;
        }

        /*
         * Mantém o conteúdo da string como aparece no JSON.
         * Não faz decodificação completa de sequências Unicode.
         */
        return duplicar_texto(valor, (size_t)(final - valor));
    }

    return NULL;
}

static int adicionar_metadados(
    Grafo *g,
    char *id,
    char *nome,
    char *nome_oficial,
    char *ref
)
{
    if (g->quantidade_vias == g->capacidade_vias) {
        int nova_capacidade = g->capacidade_vias
            ? g->capacidade_vias * 2
            : 128;

        MetadadosVia *novo = realloc(
            g->metadados_vias,
            sizeof(MetadadosVia) * (size_t)nova_capacidade
        );

        if (!novo) {
            free(id);
            free(nome);
            free(nome_oficial);
            free(ref);
            return -1;
        }

        g->metadados_vias = novo;
        g->capacidade_vias = nova_capacidade;
    }

    int indice = g->quantidade_vias++;

    g->metadados_vias[indice].id = id;
    g->metadados_vias[indice].nome = nome;
    g->metadados_vias[indice].nome_oficial = nome_oficial;
    g->metadados_vias[indice].ref = ref;

    return indice;
}

void inicializar_grafo(Grafo *g)
{
    if (!g) {
        return;
    }

    g->quantidade_arestas = 0;
    g->vertices = NULL;
    g->quantidade_vertices = 0;
    g->capacidade_vertices = 0;

    g->metadados_vias = NULL;
    g->quantidade_vias = 0;
    g->capacidade_vias = 0;
}

void destruir_grafo(Grafo *g)
{
    if (!g) {
        return;
    }

    for (int i = 0; i < g->quantidade_vertices; i++) {
        NoAdjacente *atual = g->vertices[i].adjacencias;

        while (atual) {
            NoAdjacente *prox = atual->prox;
            free(atual);
            atual = prox;
        }
    }

    free(g->vertices);

    for (int i = 0; i < g->quantidade_vias; i++) {
        free(g->metadados_vias[i].id);
        free(g->metadados_vias[i].nome);
        free(g->metadados_vias[i].nome_oficial);
        free(g->metadados_vias[i].ref);
    }

    free(g->metadados_vias);

    inicializar_grafo(g);
}

int obter_ou_criar_vertice(
    Grafo *g,
    double lon,
    double lat
)
{
    for (int i = 0; i < g->quantidade_vertices; i++) {
        Coordenada c = g->vertices[i].coordenada;

        if (fabs(c.lon - lon) < TOLERANCIA_COORDENADA &&
            fabs(c.lat - lat) < TOLERANCIA_COORDENADA) {
            return i;
        }
    }

    if (g->quantidade_vertices == g->capacidade_vertices) {
        int nova_capacidade = g->capacidade_vertices
            ? g->capacidade_vertices * 2
            : 256;

        Vertice *novo = realloc(
            g->vertices,
            sizeof(Vertice) * (size_t)nova_capacidade
        );

        if (!novo) {
            return -1;
        }

        g->vertices = novo;
        g->capacidade_vertices = nova_capacidade;
    }

    int indice = g->quantidade_vertices++;

    g->vertices[indice].coordenada.lon = lon;
    g->vertices[indice].coordenada.lat = lat;
    g->vertices[indice].adjacencias = NULL;

    return indice;
}

int adicionar_aresta_com_via(
    Grafo *g,
    int origem,
    int destino,
    TipoVia tipo,
    int id_via
)
{
    if (!g ||
        origem < 0 ||
        destino < 0 ||
        origem >= g->quantidade_vertices ||
        destino >= g->quantidade_vertices ||
        origem == destino ||
        g->quantidade_arestas == 2147483647) {
        return 0;
    }

    NoAdjacente *ida = malloc(sizeof(*ida));
    NoAdjacente *volta = malloc(sizeof(*volta));

    if (!ida || !volta) {
        free(ida);
        free(volta);
        return 0;
    }

    int id_aresta = g->quantidade_arestas;

    ida->destino = destino;
    ida->id_via = id_via;
    ida->id_aresta = id_aresta;
    ida->tipo = tipo;
    ida->prox = g->vertices[origem].adjacencias;

    volta->destino = origem;
    volta->id_via = id_via;
    volta->id_aresta = id_aresta;
    volta->tipo = tipo;
    volta->prox = g->vertices[destino].adjacencias;

    g->vertices[origem].adjacencias = ida;
    g->vertices[destino].adjacencias = volta;
    g->quantidade_arestas++;

    return 1;
}

TipoVia tipo_via_de_texto(const char *texto)
{
    if (!texto) {
        return VIA_DESCONHECIDA;
    }

    if (strcmp(texto, "residential") == 0) {
        return VIA_RESIDENCIAL;
    }

    if (strcmp(texto, "primary") == 0 ||
        strcmp(texto, "trunk") == 0 ||
        strcmp(texto, "motorway") == 0) {
        return VIA_PRINCIPAL;
    }

    if (strcmp(texto, "secondary") == 0) {
        return VIA_SECUNDARIA;
    }

    if (strcmp(texto, "tertiary") == 0) {
        return VIA_TERCIARIA;
    }

    if (strcmp(texto, "living_street") == 0 ||
        strcmp(texto, "unclassified") == 0) {
        return VIA_LOCAL;
    }

    if (strcmp(texto, "service") == 0) {
        return VIA_SERVICO;
    }

    if (strcmp(texto, "path") == 0 ||
        strcmp(texto, "footway") == 0 ||
        strcmp(texto, "track") == 0 ||
        strcmp(texto, "pedestrian") == 0) {
        return VIA_TRILHA;
    }

    return VIA_OUTRA;
}

const char *nome_tipo_via(TipoVia tipo)
{
    switch (tipo) {
        case VIA_RESIDENCIAL:
            return "residential";
        case VIA_PRINCIPAL:
            return "primary";
        case VIA_SECUNDARIA:
            return "secondary";
        case VIA_TERCIARIA:
            return "tertiary";
        case VIA_LOCAL:
            return "local";
        case VIA_SERVICO:
            return "service";
        case VIA_TRILHA:
            return "trilha";
        case VIA_OUTRA:
            return "outra";
        default:
            return "desconhecida";
    }
}

static int adicionar_feature(
    Grafo *g,
    const char *feature,
    const char *fim_feature
)
{
    const char *props = strstr(feature, "\"properties\"");
    const char *geom = strstr(feature, "\"geometry\"");

    if (!props || !geom || props >= fim_feature || geom >= fim_feature) {
        return 0;
    }

    const char *props_abre = strchr(props, '{');
    const char *props_fim = props_abre
        ? fim_objeto(props_abre, fim_feature)
        : NULL;

    const char *geom_abre = strchr(geom, '{');
    const char *geom_fim = geom_abre
        ? fim_objeto(geom_abre, fim_feature)
        : NULL;

    if (!props_abre || !props_fim ||
        !geom_abre || !geom_fim ||
        props_fim > fim_feature || geom_fim > fim_feature) {
        return 0;
    }

    char *id = extrair_string(props_abre, props_fim, "@id");
    char *nome = extrair_string(props_abre, props_fim, "name");
    char *oficial = extrair_string(
        props_abre,
        props_fim,
        "official_name"
    );
    char *ref = extrair_string(props_abre, props_fim, "ref");
    char *highway = extrair_string(props_abre, props_fim, "highway");

    TipoVia tipo = tipo_via_de_texto(highway);
    free(highway);

    const char *coordinates = strstr(geom_abre, "\"coordinates\"");

    if (!coordinates || coordinates >= geom_fim) {
        free(id);
        free(nome);
        free(oficial);
        free(ref);
        return 0;
    }

    const char *p = strchr(coordinates, ':');

    if (!p || p >= geom_fim) {
        free(id);
        free(nome);
        free(oficial);
        free(ref);
        return 0;
    }

    p = pular_espacos(p + 1, geom_fim);

    if (p >= geom_fim || *p != '[') {
        free(id);
        free(nome);
        free(oficial);
        free(ref);
        return 0;
    }

    /*
     * Confere que o GeoJSON é uma lista de coordenadas
     * com pelo menos dois pontos.
     *
     * O parser consome cada [lon, lat] inteira antes de
     * verificar a próxima vírgula ou o fechamento da lista.
     */
    const char *fim_coords = fim_array(p, geom_fim);

    if (!fim_coords) {
        free(id);
        free(nome);
        free(oficial);
        free(ref);
        return 0;
    }

    p++;
    int quantidade_pontos = 0;
    const char *verificar = p;

    while (verificar < fim_coords - 1) {
        verificar = pular_espacos(verificar, fim_coords - 1);

        if (verificar >= fim_coords - 1) {
            break;
        }

        if (*verificar == ',') {
            verificar++;
            continue;
        }

        if (*verificar != '[') {
            free(id);
            free(nome);
            free(oficial);
            free(ref);
            return 0;
        }

        double lon, lat;
        int lidos = 0;

        if (sscanf(
                verificar,
                "[ %lf , %lf ]%n",
                &lon,
                &lat,
                &lidos
            ) != 2 || lidos <= 0) {
            free(id);
            free(nome);
            free(oficial);
            free(ref);
            return 0;
        }

        quantidade_pontos++;
        verificar += lidos;
    }

    if (quantidade_pontos < 2) {
        free(id);
        free(nome);
        free(oficial);
        free(ref);
        return 0;
    }

    int id_via = adicionar_metadados(
        g,
        id,
        nome,
        oficial,
        ref
    );

    if (id_via < 0) {
        return 0;
    }

    p = pular_espacos(p, fim_coords - 1);
    int anterior = -1;

    while (p < fim_coords - 1) {
        p = pular_espacos(p, fim_coords - 1);

        if (p >= fim_coords - 1) {
            break;
        }

        if (*p == ',') {
            p++;
            continue;
        }

        double lon, lat;
        int lidos = 0;

        if (sscanf(p, "[ %lf , %lf ]%n", &lon, &lat, &lidos) != 2 ||
            lidos <= 0) {
            return 0;
        }

        int atual = obter_ou_criar_vertice(g, lon, lat);

        if (atual < 0) {
            return 0;
        }

        if (anterior >= 0 && atual != anterior) {
            if (!adicionar_aresta_com_via(
                    g,
                    anterior,
                    atual,
                    tipo,
                    id_via
                )) {
                return 0;
            }
        }

        anterior = atual;
        p += lidos;
    }

    return 1;
}

int carregar_geojson(Grafo *g, const char *caminho)
{
    if (!g || !caminho) {
        return 0;
    }

    FILE *arquivo = fopen(caminho, "rb");

    if (!arquivo) {
        perror("Erro ao abrir GeoJSON");
        return 0;
    }

    if (fseek(arquivo, 0, SEEK_END) != 0) {
        fclose(arquivo);
        return 0;
    }

    long tamanho = ftell(arquivo);

    if (tamanho < 0 || fseek(arquivo, 0, SEEK_SET) != 0) {
        fclose(arquivo);
        return 0;
    }

    char *conteudo = malloc((size_t)tamanho + 1);

    if (!conteudo) {
        fclose(arquivo);
        return 0;
    }

    size_t lidos = fread(conteudo, 1, (size_t)tamanho, arquivo);
    fclose(arquivo);

    conteudo[lidos] = '\0';

    const char *p = conteudo;
    const char *fim = conteudo + lidos;
    int features_processadas = 0;

    while (p < fim) {
        const char *feature = strstr(p, "\"type\"");

        if (!feature || feature >= fim) {
            break;
        }

        const char *abre = strchr(feature, '{');

        if (!abre || abre >= fim) {
            break;
        }

        const char *fecha = fim_objeto(abre, fim);

        if (!fecha) {
            break;
        }

        const char *tipo_feature = strstr(feature, "\"Feature\"");

        if (tipo_feature && tipo_feature < fecha) {
            if (adicionar_feature(g, abre, fecha)) {
                features_processadas++;
            }
        }

        p = fecha;
    }

    free(conteudo);

    return features_processadas > 0;
}