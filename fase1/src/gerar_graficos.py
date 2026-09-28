import matplotlib.pyplot as plt
import numpy as np

# Valores de vértices (N) testados nas issues anteriores
vertices = np.array([100, 500, 1000])

# Substitua pelos tempos reais (em milissegundos) obtidos no seu benchmark.c
tempos_algoritmo = np.array([1.2, 15.5, 78.3]) 

# Curva teórica de referência O(V + E) para comparação visual
# (Assumindo densidade proporcional, O(N) ou O(N log N))
referencia_linear = vertices * 0.08

plt.figure(figsize=(8, 5))
plt.plot(vertices, tempos_algoritmo, marker='o', linestyle='-', color='b', label='Algoritmo Real (Medido)')
plt.plot(vertices, referencia_linear, linestyle='--', color='gray', label='Tendência Teórica O(V+E)')

plt.title('Análise de Crescimento Assintótico - Desempenho')
plt.xlabel('Tamanho do Grafo (Vértices N)')
plt.ylabel('Tempo de Execução (ms)')
plt.legend()
plt.grid(True)

# Guardar o gráfico como imagem para incluir no relatório do projeto
plt.savefig('fase1/data/grafico_crescimento.png', dpi=300)
print("Gráfico gerado e guardado com sucesso em fase1/data/grafico_crescimento.png!")
plt.show()