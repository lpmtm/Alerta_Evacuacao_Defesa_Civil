#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"

/* ---------------- Cronômetro ---------------- */

void iniciar_cronometro(Cronometro *c) {
    c->inicio = (double)clock() / CLOCKS_PER_SEC;
}

double parar_cronometro(Cronometro *c) {
    double fim = (double)clock() / CLOCKS_PER_SEC;
    double segundos = fim - c->inicio;
    return segundos * 1000.0; /* converte para milissegundos */
}

/* ---------------- Memória ---------------- */

/*
 * Estratégia: cada bloco alocado por log_malloc/log_calloc guarda,
 * escondido logo antes do endereço retornado, o tamanho que foi
 * pedido. Assim, na hora de liberar (log_free), sabemos exatamente
 * quantos bytes devolver ao contador -- sem depender de nenhuma
 * biblioteca externa.
 *
 * Layout de cada bloco alocado:
 * [ tamanho (size_t) ][ dados retornados ao usuário ]
 *                      ^-- ponteiro retornado por log_malloc
 */

static long g_memoria_atual = 0;
static long g_memoria_pico = 0;

static void atualizar_pico(void) {
    if (g_memoria_atual > g_memoria_pico) {
        g_memoria_pico = g_memoria_atual;
    }
}

void *log_malloc(size_t tamanho) {
    void *bloco = malloc(sizeof(size_t) + tamanho);
    if (!bloco) return NULL;

    *(size_t *)bloco = tamanho;
    g_memoria_atual += (long)tamanho;
    atualizar_pico();

    return (char *)bloco + sizeof(size_t);
}

void *log_calloc(size_t quantidade, size_t tamanho_item) {
    size_t total = quantidade * tamanho_item;
    void *ponteiro = log_malloc(total);
    if (ponteiro) {
        memset(ponteiro, 0, total);
    }
    return ponteiro;
}

void *log_realloc(void *ptr, size_t novo_tamanho) {
    if (!ptr) {
        return log_malloc(novo_tamanho);
    }

    char *bloco_antigo = (char *)ptr - sizeof(size_t);
    size_t tamanho_antigo = *(size_t *)bloco_antigo;

    void *novo_bloco = realloc(bloco_antigo, sizeof(size_t) + novo_tamanho);
    if (!novo_bloco) return NULL;

    *(size_t *)novo_bloco = novo_tamanho;

    g_memoria_atual += (long)novo_tamanho - (long)tamanho_antigo;
    atualizar_pico();

    return (char *)novo_bloco + sizeof(size_t);
}

void log_free(void *ptr) {
    if (!ptr) return;

    char *bloco = (char *)ptr - sizeof(size_t);
    size_t tamanho = *(size_t *)bloco;

    g_memoria_atual -= (long)tamanho;
    free(bloco);
}

long memoria_atual_bytes(void) {
    return g_memoria_atual;
}

long memoria_pico_bytes(void) {
    return g_memoria_pico;
}

/* ---------------- Registro em log ---------------- */

void registrar_log(const char *nome_algoritmo, int n_vertices,
                    double tempo_ms, long memoria_bytes) {

    /* Mostra no console */
    printf("[LOG] Algoritmo=%s | N=%d | Tempo=%.3f ms | Memoria=%ld bytes (%.2f MB)\n",
           nome_algoritmo, n_vertices, tempo_ms, memoria_bytes,
           memoria_bytes / (1024.0 * 1024.0));

    /* Garante que a pasta "logs" existe (cria se necessário) */
    #if defined(_WIN32)
        system("if not exist logs mkdir logs");
    #else
        system("mkdir -p logs");
    #endif

    /* Verifica se o arquivo já existe, para decidir se escreve o cabeçalho */
    FILE *teste = fopen("logs/resultados.csv", "r");
    int arquivo_existe = (teste != NULL);
    if (teste) fclose(teste);

    FILE *arquivo = fopen("logs/resultados.csv", "a");
    if (!arquivo) {
        fprintf(stderr, "Aviso: não foi possível abrir logs/resultados.csv para escrita.\n");
        return;
    }

    if (!arquivo_existe) {
        fprintf(arquivo, "algoritmo,n_vertices,tempo_ms,memoria_bytes\n");
    }

    fprintf(arquivo, "%s,%d,%.3f,%ld\n", nome_algoritmo, n_vertices, tempo_ms, memoria_bytes);
    fclose(arquivo);
}
