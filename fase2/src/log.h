#ifndef LOG_H
#define LOG_H

#include <stddef.h>

/*
 * Sistema de Log de Desempenho (RF03)
 * ------------------------------------
 * Módulo compartilhado por todos os algoritmos do projeto
 * (BFS, DFS, Tarjan, Dijkstra, Cobertura de Vértices, etc.)
 * para medir e registrar tempo de execução (ms) e consumo de
 * memória (bytes) de cada execução.
 *
 * COMO USAR (exemplo dentro do seu algoritmo):
 *
 *   Cronometro c;
 *   iniciar_cronometro(&c);
 *
 *   // ... roda seu algoritmo aqui ...
 *
 *   double tempo_ms = parar_cronometro(&c);
 *   long memoria = memoria_atual_bytes();
 *
 *   registrar_log("BFS", numero_de_vertices, tempo_ms, memoria);
 *
 * Para que a contagem de memória funcione, troque as chamadas de
 * malloc/calloc/free do SEU algoritmo por log_malloc/log_calloc/log_free.
 */

/* ---------------- Cronômetro ---------------- */

typedef struct {
    double inicio; /* instante inicial, em segundos (uso interno) */
} Cronometro;

/* Marca o instante inicial */
void iniciar_cronometro(Cronometro *c);

/* Retorna o tempo decorrido desde iniciar_cronometro, em milissegundos */
double parar_cronometro(Cronometro *c);

/* ---------------- Memória ---------------- */

/* Alocação de memória rastreada (substitui malloc) */
void *log_malloc(size_t tamanho);

/* Alocação de memória zerada e rastreada (substitui calloc) */
void *log_calloc(size_t quantidade, size_t tamanho_item);

/* Realocação rastreada (substitui realloc) */
void *log_realloc(void *ptr, size_t novo_tamanho);

/* Libera memória rastreada (substitui free) */
void log_free(void *ptr);

/* Memória atualmente alocada pelo programa, em bytes (via log_malloc/etc.) */
long memoria_atual_bytes(void);

/* Pico máximo de memória já utilizado durante a execução, em bytes */
long memoria_pico_bytes(void);

/* ---------------- Registro em log ---------------- */

/*
 * Registra uma linha de log no console E em um arquivo CSV
 * (logs/resultados.csv), no formato:
 * algoritmo,n_vertices,tempo_ms,memoria_bytes
 *
 * O arquivo CSV é criado automaticamente se não existir, e novas
 * execuções são adicionadas ao final (sem apagar as anteriores),
 * o que facilita gerar os gráficos de crescimento (Protocolo Experimental).
 */
void registrar_log(const char *nome_algoritmo, int n_vertices,
                    double tempo_ms, long memoria_bytes);

#endif
