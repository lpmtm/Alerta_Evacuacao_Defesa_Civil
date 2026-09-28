#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"

int main() {
    printf("=== Comparação de Consumo de Memória (Lista vs Matriz) ===\n");

    int n_valores[] = {100, 500, 1000};
    
    for (int i = 0; i < 3; i++) {
        int N = n_valores[i];
        
        // Cálculo teórico/prático adaptado aos tipos do projeto
        size_t mem_matriz = (size_t)N * N * sizeof(int); // Matriz de adjacência (N x N)
        size_t mem_lista = sizeof(Grafo) + (size_t)(2 * N * sizeof(int)); // Estimativa para lista
        
        printf("\nPara N = %d vértices:\n", N);
        printf("- Matriz de Adjacência: %zu bytes (%.2f KB)\n", mem_matriz, mem_matriz / 1024.0);
        printf("- Lista de Adjacência:  %zu bytes (%.2f KB)\n", mem_lista, mem_lista / 1024.0);
    }

    return 0;
}