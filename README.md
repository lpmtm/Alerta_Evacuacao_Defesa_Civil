# Alerta Evacuação Defesa Civil

Análise da malha viária do entorno da mina **Córrego do Feijão (Brumadinho–MG)**, onde a barragem B1 se rompeu em 25/01/2019, usando Teoria dos Grafos. O projeto modela cruzamentos como vértices e trechos de via como arestas para responder perguntas da Defesa Civil: quais regiões estão isoladas, quais vias não têm desvio, quais cruzamentos são pontos únicos de falha e em que ordem a frente de rejeitos alcança cada ponto (Fase I); e, com pesos nas arestas, qual a rota mais rápida até um ponto seguro e onde instalar o menor número de postos de socorro (Fase II).

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

## Fase II em andamento

| Resultado parcial | Valor |
|---|---|
| Pesos das arestas | Haversine × fator de lentidão por tipo de via ([tabela](fase2/tabela_lentidao.md)) |
| Malha efetiva | 88,3 km reais equivalem a 207,0 km na via de referência (×2,34) |
| Propagação a partir da B1 | 3.115 vértices atingidos em 257 camadas |
| Pontes com metadados OSM | 1.615 arestas-ponte, relatório em [`fase2/saida/`](fase2/saida/) |
| Subgrafos de teste | N = 100, 500 e 1.000 vértices |
| Cobertura de Vértices | solução exata por backtracking implementada |

Situação de cada item e o que falta (Dijkstra, heurística, testes de estresse): [`fase2/README.md`](fase2/README.md#andamento-da-fase-ii).

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
├── fase2/                     Otimização e Complexidade (detalhes em fase2/README.md)
│   ├── Makefile               make / make run T=nome / make clean
│   ├── tabela_lentidao.md     Justificativa dos pesos por tipo de via
│   ├── src/                   grafo, log, pesos (Haversine + lentidão), geo, propagacao,
│   │                          ponte, subgrafos, cobertura e os programas main_*.c
│   ├── tests/                 teste_*.c de cada módulo
│   ├── data/                  mapa_brumadinho.geojson
│   └── saida/                 Relatórios gerados (pontes, propagação, subgrafos, cobertura)
└── docs/
    ├── apresentacao/          Slides
    └── artigo/                Artigo SBC
        ├── artigo.tex / .pdf  Fase I
        ├── figuras/           Figuras da Fase I
        └── fase2/             Artigo da Fase II (artigo_fase2.tex, figuras/)
```

## Como compilar e executar

Requisitos: GCC (ou MinGW no Windows).

### Fase II

Dentro de `fase2/`, com `make` (no Windows, `mingw32-make`):

```bash
make                         # compila src/main_*.c e tests/teste_*.c em bin/
make run T=main_propagacao   # roda um executável com o dataset
make clean
```

A lista de executáveis e o que cada um gera está em [`fase2/README.md`](fase2/README.md#como-compilar-e-executar).

### Fase I

Todos os comandos abaixo são executados dentro de `fase1/src`.

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

### Artigos

- Fase I: `cd docs/artigo && lualatex artigo.tex` (rodar duas vezes).
- Fase II: `cd docs/artigo/fase2 && lualatex artigo_fase2.tex` (rodar duas vezes).

Os dois também compilam com pdfLaTeX, por exemplo no Overleaf (subir o `.tex` junto com a pasta `figuras/`).

## Dados

Malha viária extraída do [OpenStreetMap](https://www.openstreetmap.org) via overpass-turbo em 08/09/2026 (licença ODbL). Vértices são coordenadas distintas; arestas ligam coordenadas consecutivas de uma mesma via. O grafo é não direcionado; na Fase I é não ponderado e, na Fase II, cada aresta guarda também o tipo da via (`highway`) usado no peso.

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
- **Fase II — Otimização e Complexidade:** em andamento. Concluídos: tipo da via, distância de Haversine, tabela de lentidão, vértice mais próximo da B1, propagação, pontes com metadados, subgrafos de teste e Cobertura de Vértices exata. Faltam: componente de risco no peso, Dijkstra para rotas seguras, heurística da Cobertura e comparação com a exata, matriz de adjacência (RF02), testes de estresse e o artigo.
