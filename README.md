# Alerta Evacuação Defesa Civil

Simulação computacional do rompimento de uma barragem de rejeitos, aplicada a um cenário de evacuação e defesa civil. O projeto utiliza Teoria dos Grafos para modelar a propagação do desastre, identificar rotas de fuga críticas e otimizar o posicionamento de recursos de resposta.

## Cenário

Mapeamento de uma zona de risco (enchente/desabamento) inspirado em casos reais de rompimento de barragem, como o de Brumadinho. O grafo representa cruzamentos e vias de uma cidade/região, permitindo:

- Identificar pontes e vias críticas que precisam de reforço estrutural preventivo
- Simular a propagação do desastre a partir do ponto de ruptura
- Calcular rotas mais seguras para ambulâncias até hospitais
- Posicionar postos móveis da Defesa Civil de forma eficiente

## Estrutura do repositório

```
.
├── fase1/          Topologia e Conectividade (BFS/DFS, pontes, articulação)
│   ├── src/        Código-fonte em C
│   ├── data/       Datasets utilizados
│   └── tests/      Scripts e resultados de testes de desempenho
├── fase2/          Otimização e Complexidade (Dijkstra, Cobertura de Vértices)
│   ├── src/
│   ├── data/
│   └── tests/
└── docs/
    └── artigo/     Artigo científico no padrão SBC
```

## Requisitos técnicos

- Linguagem: **C**, sem bibliotecas prontas de grafos (implementação autoral dos algoritmos core)
- Dataset real com no mínimo 1.000 vértices
- Suporte a Lista e Matriz de Adjacência
- Geração de logs de tempo de execução (ms) e consumo de memória

## Como compilar e rodar

```bash
cd fase1/src
gcc -o programa main.c -Wall
./programa
```

*(Instruções detalhadas serão atualizadas conforme os módulos forem implementados.)*

## Equipe

| Pessoa | Responsabilidade | Responsável |
|---|---|---|
| P1 | Estrutura de dados (Lista/Matriz de Adjacência), leitura de dataset, logging | Caroline Lopes Martins |
| P2 | BFS/DFS, conectividade, simulação de propagação do desastre | Ian Melo Gonçalves |
| P3 | Pontes e Vértices de Articulação (Tarjan) | Matheus Filipe da Silva Ponte |
| P4 | Testes de desempenho e gráficos comparativos | Guilherme Amaro de Castro |
| P5 | Documentação, artigo científico e apresentação | Bruno Nobrega Souza |

## Fases do projeto

- **Fase I:** Topologia e Conectividade — Sprint 1 e 2
- **Fase II:** Otimização e Complexidade — a definir

## Disciplina

Projeto acadêmico aplicando Teoria dos Grafos a um problema real, com artigo científico no padrão SBC (Sociedade Brasileira de Computação).
