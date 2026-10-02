# Alerta Evacuação Defesa Civil

Análise da malha viária do entorno da mina **Córrego do Feijão (Brumadinho–MG)**, onde a barragem B1 se rompeu em 25/01/2019, usando Teoria dos Grafos. O projeto modela cruzamentos como vértices e trechos de via como arestas para responder perguntas da Defesa Civil: quais regiões estão isoladas, quais vias não têm desvio, quais cruzamentos são pontos únicos de falha e em que ordem a frente de rejeitos alcança cada ponto.

Projeto integrador da disciplina de Teoria dos Grafos — implementação autoral em **C**, sem bibliotecas de grafos (RNF01), com artigo no padrão SBC.

## Principais resultados da Fase I

| Resultado | Valor |
|---|---|
| Grafo (OpenStreetMap, 224 vias) | 3.143 vértices, 3.199 arestas, 88,3 km |
| Componentes conexos | 3 (principal com 3.115 vértices) |
| Pontes (vias sem rota alternativa) | 1.615 arestas (50,5%) — **145 trechos críticos** |
| Vértices de articulação | 1.577 — **107 cruzamentos reais** |
| Ciclos / bipartição | possui ciclos (59 independentes) / não é bipartido |
| Memória no grafo completo | lista 124,5 KiB × matriz 37,7 MiB (310×) |
| Crescimento empírico das buscas | linear (expoente entre 0,93 e 1,09) |

Todos os resultados estruturais foram validados com a biblioteca NetworkX sobre a mesma lista de arestas. Detalhes em [`docs/artigo/artigo.pdf`](docs/artigo/artigo.pdf).

## Estrutura do repositório

```
.
├── fase1/                     Topologia e Conectividade
│   ├── src/                   Código-fonte em C
│   │   ├── grafo.c/.h         Estrutura (lista + matriz), leitura do GeoJSON
│   │   ├── log.c/.h           Log de tempo (ms) e memória (RF03)
│   │   ├── bfs.c, dfs.c       Buscas em largura e profundidade
│   │   ├── componentes.c      Componentes conexos e vértices isolados
│   │   ├── ciclos.c           Detecção de ciclos e bipartição
│   │   ├── propagacao.c       Propagação da lama por níveis da BFS
│   │   ├── tarjan.c           Pontes e vértices de articulação
│   │   ├── gerar_subgrafos.c  Subgrafos de 100/500/1000 vértices (CSV)
│   │   └── teste_*.c          Programas de teste de cada módulo
│   ├── data/                  mapa_brumadinho.geojson, subgrafos CSV, relatório de vias críticas
│   └── tests/                 Scripts e resultados dos testes de desempenho
├── fase2/                     Otimização e Complexidade (Dijkstra, Cobertura de Vértices)
└── docs/
    └── artigo/                Artigo SBC (artigo.tex, artigo.pdf, figuras/)
```

## Como compilar e executar

Requisitos: GCC (ou MinGW no Windows). Todos os comandos abaixo são executados dentro de `fase1/src`.

**Programa principal** (carrega o mapa e mostra estatísticas):

```bash
gcc -o programa main.c grafo.c -lm -Wall
./programa ../data/mapa_brumadinho.geojson
```

**Testes de cada módulo:**

```bash
gcc -o teste_bfs         teste_bfs.c bfs.c grafo.c log.c -lm
gcc -o teste_dfs         teste_dfs.c dfs.c grafo.c log.c -lm
gcc -o teste_componentes teste_componentes.c componentes.c bfs.c grafo.c log.c -lm
gcc -o teste_ciclos      teste_ciclos.c ciclos.c grafo.c log.c -lm
gcc -o teste_propagacao  teste_propagacao.c propagacao.c bfs.c grafo.c log.c -lm
gcc -o teste_tarjan      teste_tarjan.c tarjan.c grafo.c log.c -lm   # gera data/relatorio_vias_criticas.txt
```

Cada algoritmo registra tempo e memória em `logs/resultados.csv` (RF03).

**Artigo:** `cd docs/artigo && lualatex artigo.tex` (rodar duas vezes). Também compila com pdfLaTeX, por exemplo no Overleaf.

## Dados

Malha viária extraída do [OpenStreetMap](https://www.openstreetmap.org) via overpass-turbo em 08/09/2026 (licença ODbL). Vértices são coordenadas distintas; arestas ligam coordenadas consecutivas de uma mesma via. O grafo é não direcionado e, na Fase I, não ponderado.

## Equipe

| Pessoa | Responsabilidade | Responsável |
|---|---|---|
| P1 | Estrutura de dados (Lista/Matriz de Adjacência), leitura de dataset, logging | Caroline Lopes Martins |
| P2 | BFS/DFS, conectividade, simulação de propagação do desastre | Ian Melo Gonçalves |
| P3 | Pontes e Vértices de Articulação (Tarjan) | Matheus Filipe da Silva Ponte |
| P4 | Testes de desempenho e gráficos comparativos | Guilherme Amaro de Castro |
| P5 | Documentação, artigo científico e apresentação | Bruno Nobrega Souza |

## Fases do projeto

- **Fase I — Topologia e Conectividade:** concluída (estrutura de dados, BFS/DFS, componentes, ciclos/bipartição, propagação, Tarjan, testes de desempenho e artigo).
- **Fase II — Otimização e Complexidade:** pesos pela distância de Haversine, Dijkstra para rotas seguras e Cobertura de Vértices para postos de socorro.
