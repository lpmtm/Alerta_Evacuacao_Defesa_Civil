
#include <stdio.h>
#include <stdlib.h>

#include "../src/grafo.h"
#include "../src/ponte.h"

static int contar_linhas_csv(const char *caminho)
{
    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo) {
        perror("Erro ao abrir CSV");
        return -1;
    }

    int linhas = 0;
    int caractere;

    while ((caractere = fgetc(arquivo)) != EOF) {
        if (caractere == '\n') {
            linhas++;
        }
    }

    fclose(arquivo);
    return linhas > 0 ? linhas - 1 : 0;
}

static int adicionar_teste(Grafo *g, int u, int v)
{
    return adicionar_aresta_com_via(
        g, u, v, VIA_LOCAL, -1
    );
}

static int executar_teste(
    const char *nome,
    Grafo *g,
    int esperadas
)
{
    const char *csv = "tests/resultado_teste.csv";
    const char *txt = "tests/resultado_teste.txt";

    printf("\nTeste: %s\n", nome);

    if (!gerar_relatorios_pontes(g, csv, txt)) {
        printf("FALHOU: erro ao gerar relatorios.\n");
        remove(csv);
        remove(txt);
        return 0;
    }

    int encontradas = contar_linhas_csv(csv);

    remove(csv);
    remove(txt);

    if (encontradas != esperadas) {
        printf(
            "FALHOU: esperadas %d, encontradas %d.\n",
            esperadas, encontradas
        );
        return 0;
    }

    printf("PASSOU: %d ponte(s).\n", encontradas);
    return 1;
}

static int criar_grafo_teste(Grafo *g, int quantidade)
{
    inicializar_grafo(g);

    for (int i = 0; i < quantidade; i++) {
        if (obter_ou_criar_vertice(g, (double)i, 0.0) < 0) {
            destruir_grafo_conteudo(g);
            return 0;
        }
    }

    return 1;
}

int main(void)
{
    int aprovados = 0;
    const int total = 5;
    Grafo g;
    int ok;

    /* 1. Caminho A-B-C: duas pontes. */
    if (!criar_grafo_teste(&g, 3)) return EXIT_FAILURE;

    ok = adicionar_teste(&g, 0, 1) &&
         adicionar_teste(&g, 1, 2);

    if (ok) {
        aprovados += executar_teste("Caminho A-B-C", &g, 2);
    } else {
        puts("FALHOU: insercao de aresta no teste 1.");
    }
    destruir_grafo_conteudo(&g);

    /* 2. Triangulo: nenhuma ponte. */
    if (!criar_grafo_teste(&g, 3)) return EXIT_FAILURE;

    ok = adicionar_teste(&g, 0, 1) &&
         adicionar_teste(&g, 1, 2) &&
         adicionar_teste(&g, 2, 0);

    if (ok) {
        aprovados += executar_teste("Triangulo", &g, 0);
    } else {
        puts("FALHOU: insercao de aresta no teste 2.");
    }
    destruir_grafo_conteudo(&g);

    /* 3. Triangulo com ramo C-D: uma ponte. */
    if (!criar_grafo_teste(&g, 4)) return EXIT_FAILURE;

    ok = adicionar_teste(&g, 0, 1) &&
         adicionar_teste(&g, 1, 2) &&
         adicionar_teste(&g, 2, 0) &&
         adicionar_teste(&g, 2, 3);

    if (ok) {
        aprovados += executar_teste("Triangulo com ramo", &g, 1);
    } else {
        puts("FALHOU: insercao de aresta no teste 3.");
    }
    destruir_grafo_conteudo(&g);

    /* 4. Caminho com vertice isolado: duas pontes. */
    if (!criar_grafo_teste(&g, 4)) return EXIT_FAILURE;

    ok = adicionar_teste(&g, 0, 1) &&
         adicionar_teste(&g, 1, 2);

    if (ok) {
        aprovados += executar_teste(
            "Grafo desconectado com vertice isolado", &g, 2
        );
    } else {
        puts("FALHOU: insercao de aresta no teste 4.");
    }
    destruir_grafo_conteudo(&g);

    /*
     * 5. Duas arestas paralelas A-B e uma aresta B-C.
     * As paralelas nao sao pontes; somente B-C e ponte.
     */
    if (!criar_grafo_teste(&g, 3)) return EXIT_FAILURE;

    ok = adicionar_teste(&g, 0, 1) &&
         adicionar_teste(&g, 0, 1) &&
         adicionar_teste(&g, 1, 2);

    if (ok) {
        if (g.quantidade_arestas != 3) {
            puts("FALHOU: quantidade de arestas deveria ser 3.");
        } else {
            aprovados += executar_teste(
                "Arestas paralelas A-B e ramo B-C", &g, 1
            );
        }
    } else {
        puts("FALHOU: insercao de aresta no teste 5.");
    }
    destruir_grafo_conteudo(&g);

    printf("\n============================\n");
    printf("Resultado: %d/%d testes aprovados.\n", aprovados, total);
    printf("============================\n");

    return aprovados == total ? EXIT_SUCCESS : EXIT_FAILURE;
}
