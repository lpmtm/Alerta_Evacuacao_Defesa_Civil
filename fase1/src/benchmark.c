#include <stdio.h>
#include <time.h>
#include "grafo.h"

// Função auxiliar declarada fora do main
void testar_algoritmo(const char* nome, void (*funcao)(), const char* arquivo_csv) {
    // Aqui carregaríamos o subgrafo do arquivo_csv conforme a lógica da equipa
    clock_t inicio = clock();

    // Executar o algoritmo a testar
    funcao();

    clock_t fim = clock();
    double tempo_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;

    printf("[%s] Ficheiro: %s | Tempo: %.3f ms\n", nome, arquivo_csv, tempo_ms);
}

// Exemplo de função mock/stub para simular o algoritmo de P1, P2 ou P3
void algoritmo_exemplo() {
    // Simulação de processamento do grafo
    for(volatile int i = 0; i < 1000000; i++);
}

int main() {
    printf("Iniciando benchmarks para a Issue #19...\n");

    // Testando com os três tamanhos de subgrafos gerados na Issue #17
    testar_algoritmo("Algoritmo_P1", algoritmo_exemplo, "fase1/data/subgrafo_100.csv");
    testar_algoritmo("Algoritmo_P1", algoritmo_exemplo, "fase1/data/subgrafo_500.csv");
    testar_algoritmo("Algoritmo_P1", algoritmo_exemplo, "fase1/data/subgrafo_1000.csv");

    return 0;
}