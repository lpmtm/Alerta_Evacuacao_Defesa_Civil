# Alerta Evacuação Defesa Civil

Simulação computacional do rompimento de uma barragem de rejeitos, aplicada a um cenário de evacuação e defesa civil. O projeto utiliza Teoria dos Grafos para modelar a propagação do desastre, identificar rotas de fuga críticas e otimizar o posicionamento de recursos de resposta.

## Cenário

Mapeamento de uma zona de risco (enchente/desabamento) inspirado em casos reais de rompimento de barragem, como o de Brumadinho. O grafo representa cruzamentos e vias de uma cidade/região, permitindo:

- Identificar pontes e vias críticas que precisam de reforço estrutural preventivo
- Simular a propagação do desastre a partir do ponto de ruptura
- Calcular rotas mais seguras para ambulâncias até hospitais
- Posicionar postos móveis da Defesa Civil de forma eficiente

## Estrutura do repositório

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
