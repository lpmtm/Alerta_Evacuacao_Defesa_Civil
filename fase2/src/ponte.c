#include "ponte.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    Grafo *grafo;

    FILE *arquivo_csv;
    FILE *arquivo_txt;

    int *descoberta;
    int *low;
    int *pai;
    int *pai_aresta;

    int tempo;
    int total_pontes;
    int pontes_com_nome;
    int pontes_com_nome_oficial;
} ContextoTarjan;

static int escrever_csv(FILE *arquivo, const char *texto)
{
    if (!texto) {
        texto = "";
    }

    if (fputc('"', arquivo) == EOF) {
        return 0;
    }

    for (const char *p = texto; *p; p++) {
        if (*p == '"') {
            if (fputc('"', arquivo) == EOF ||
                fputc('"', arquivo) == EOF) {
                return 0;
            }
        } else if (fputc(*p, arquivo) == EOF) {
            return 0;
        }
    }

    return fputc('"', arquivo) != EOF;
}

static int texto_preenchido(const char *texto)
{
    if (!texto) {
        return 0;
    }

    while (*texto) {
        if (*texto != ' ' && *texto != '\t' &&
            *texto != '\r' && *texto != '\n') {
            return 1;
        }

        texto++;
    }

    return 0;
}

static int escrever_linha_csv(
    ContextoTarjan *ctx,
    int origem,
    int destino,
    TipoVia tipo,
    int id_via
)
{
    Grafo *g = ctx->grafo;

    Coordenada a = g->vertices[origem].coordenada;
    Coordenada b = g->vertices[destino].coordenada;

    const char *id = "";
    const char *nome = "";
    const char *oficial = "";
    const char *ref = "";

    if (id_via >= 0 && id_via < g->quantidade_vias) {
        MetadadosVia *m = &g->metadados_vias[id_via];

        id = m->id ? m->id : "";
        nome = m->nome ? m->nome : "";
        oficial = m->nome_oficial ? m->nome_oficial : "";
        ref = m->ref ? m->ref : "";
    }

    if (fprintf(
            ctx->arquivo_csv,
            "%d,%d,%.7f,%.7f,%.7f,%.7f,",
            origem,
            destino,
            a.lon,
            a.lat,
            b.lon,
            b.lat
        ) < 0) {
        return 0;
    }

    if (!escrever_csv(ctx->arquivo_csv, nome) ||
        fputc(',', ctx->arquivo_csv) == EOF ||
        !escrever_csv(ctx->arquivo_csv, oficial) ||
        fputc(',', ctx->arquivo_csv) == EOF ||
        !escrever_csv(ctx->arquivo_csv, ref) ||
        fputc(',', ctx->arquivo_csv) == EOF ||
        !escrever_csv(ctx->arquivo_csv, id) ||
        fprintf(
            ctx->arquivo_csv,
            ",%s,aresta_ponte_topologica\n",
            nome_tipo_via(tipo)
        ) < 0) {
        return 0;
    }

    return 1;
}

static int escrever_linha_relatorio(
    ContextoTarjan *ctx,
    int origem,
    int destino,
    TipoVia tipo,
    int id_via
)
{
    Grafo *g = ctx->grafo;

    const char *id = "";
    const char *nome = "";
    const char *oficial = "";
    const char *ref = "";

    if (id_via >= 0 && id_via < g->quantidade_vias) {
        MetadadosVia *m = &g->metadados_vias[id_via];

        id = m->id ? m->id : "";
        nome = m->nome ? m->nome : "";
        oficial = m->nome_oficial ? m->nome_oficial : "";
        ref = m->ref ? m->ref : "";
    }

    if (texto_preenchido(nome)) {
        ctx->pontes_com_nome++;
    }

    if (texto_preenchido(oficial)) {
        ctx->pontes_com_nome_oficial++;
    }

    Coordenada a = g->vertices[origem].coordenada;
    Coordenada b = g->vertices[destino].coordenada;

    if (fprintf(
            ctx->arquivo_txt,
            "- Vértices %d e %d | Via: %s | Nome oficial: %s"
            " | Referência: %s | OSM: %s | Tipo: %s\n"
            "  Coordenadas: (%.7f, %.7f) -> (%.7f, %.7f)\n",
            origem,
            destino,
            texto_preenchido(nome) ? nome : "(sem nome)",
            texto_preenchido(oficial) ? oficial : "(sem nome oficial)",
            texto_preenchido(ref) ? ref : "(sem referência)",
            texto_preenchido(id) ? id : "(sem ID)",
            nome_tipo_via(tipo),
            a.lon,
            a.lat,
            b.lon,
            b.lat
        ) < 0) {
        return 0;
    }

    return 1;
}

static int dfs_tarjan(ContextoTarjan *ctx, int u, int aresta_pai)
{
    ctx->descoberta[u] = ctx->low[u] = ++ctx->tempo;

    for (NoAdjacente *adj = ctx->grafo->vertices[u].adjacencias;
         adj;
         adj = adj->prox) {

        int v = adj->destino;

        /* Ignora apenas a aresta usada para chegar a u. */
        if (adj->id_aresta == aresta_pai) {
            continue;
        }

        if (ctx->descoberta[v] == 0) {
            ctx->pai[v] = u;
            ctx->pai_aresta[v] = adj->id_aresta;

            if (!dfs_tarjan(ctx, v, adj->id_aresta)) {
                return 0;
            }

            if (ctx->low[v] < ctx->low[u]) {
                ctx->low[u] = ctx->low[v];
            }

            if (ctx->low[v] > ctx->descoberta[u]) {
                ctx->total_pontes++;

                if (!escrever_linha_csv(
                        ctx, u, v, adj->tipo, adj->id_via)) {
                    return 0;
                }

                if (!escrever_linha_relatorio(
                        ctx, u, v, adj->tipo, adj->id_via)) {
                    return 0;
                }
            }
        } else if (ctx->descoberta[v] < ctx->low[u]) {
            ctx->low[u] = ctx->descoberta[v];
        }
    }

    return 1;
}

int gerar_relatorios_pontes(
    Grafo *g,
    const char *caminho_csv,
    const char *caminho_txt
)
{
    if (!g || !caminho_csv || !caminho_txt) {
        return 0;
    }

    FILE *csv = fopen(caminho_csv, "w");

    if (!csv) {
        perror("Erro ao criar CSV");
        return 0;
    }

    FILE *txt = fopen(caminho_txt, "w");

    if (!txt) {
        perror("Erro ao criar relatório TXT");
        fclose(csv);
        return 0;
    }

    fprintf(
        csv,
        "id_origem,id_destino,lon_origem,lat_origem,"
        "lon_destino,lat_destino,nome_via,nome_oficial,"
        "ref_via,id_osm,tipo_via,criterio\n"
    );

    fprintf(
        txt,
        "RELATÓRIO DE PONTES TOPOLÓGICAS\n"
        "================================\n\n"
        "O relatório identifica arestas cuja remoção desconecta "
        "componentes do grafo rodoviário.\n\n"
        "IMPORTANTE: uma ponte topológica não é necessariamente "
        "uma ponte física nem prova, por si só, que uma rua seja "
        "a única rota de fuga de um bairro. A conclusão depende "
        "da região de origem, do destino de evacuação e da "
        "completude do mapa.\n\n"
    );

    ContextoTarjan ctx = {0};

    ctx.grafo = g;
    ctx.arquivo_csv = csv;
    ctx.arquivo_txt = txt;

    int n = g->quantidade_vertices;

    ctx.descoberta = calloc((size_t)n, sizeof(int));
    ctx.low = calloc((size_t)n, sizeof(int));
    ctx.pai = malloc(sizeof(int) * (size_t)n);
    ctx.pai_aresta = malloc(sizeof(int) * (size_t)n);

    if (n > 0 && (!ctx.descoberta || !ctx.low || !ctx.pai)) {
        free(ctx.descoberta);
        free(ctx.low);
        free(ctx.pai);

        fclose(csv);
        fclose(txt);
        return 0;
    }

    for (int i = 0; i < n; i++) {
        ctx.pai[i] = -1;
    }

    int sucesso = 1;

    for (int i = 0; i < n && sucesso; i++) {
        if (ctx.descoberta[i] == 0) {
            sucesso = dfs_tarjan(&ctx, i, -1);
        }
    }

    fprintf(
        txt,
        "\nRESUMO\n"
        "------\n"
        "Vértices do grafo: %d\n"
        "Vias com metadados: %d\n"
        "Arestas-ponte encontradas: %d\n"
        "Pontes com nome de via: %d\n"
        "Pontes com nome oficial: %d\n",
        g->quantidade_vertices,
        g->quantidade_vias,
        ctx.total_pontes,
        ctx.pontes_com_nome,
        ctx.pontes_com_nome_oficial
    );

    free(ctx.descoberta);
    free(ctx.low);
    free(ctx.pai);

    int erro_escrita = ferror(csv) || ferror(txt);

    if (fclose(csv) != 0) {
        erro_escrita = 1;
    }

    if (fclose(txt) != 0) {
        erro_escrita = 1;
    }

    return sucesso && !erro_escrita;
}