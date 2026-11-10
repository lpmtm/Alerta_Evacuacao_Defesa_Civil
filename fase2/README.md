# Fase II — Otimização e Complexidade

Nesta fase o grafo da malha viária de Córrego do Feijão (Brumadinho–MG) deixa de ser apenas topológico: cada aresta recebe um **peso** que traduz o custo real de percorrer o trecho, e o grafo passa a responder perguntas de **otimização**:

- **Por onde a ambulância chega mais rápido / por onde a população foge?** → menor caminho (Dijkstra).
- **Onde instalar o menor número de postos de socorro de modo que toda via tenha um posto numa das pontas?** → Cobertura de Vértices (problema NP-difícil), com solução exata e heurística.

A base (estrutura do grafo, leitura do GeoJSON e log) foi copiada da Fase I e estendida. O código continua autoral, em C, sem bibliotecas de grafos (RNF01).

## Estrutura da pasta

```
fase2/
├── Makefile                 Compila tudo (src/main_*.c e tests/teste_*.c) para bin/
├── tabela_lentidao.md       Justificativa do fator de lentidão por tipo de via (apoio ao artigo)
├── data/
│   └── mapa_brumadinho.geojson    Mesmo dataset da Fase I (OpenStreetMap, 224 vias)
├── src/
│   ├── grafo.c/.h           Lista de adjacência + tipo da via (highway) e metadados OSM por aresta
│   ├── log.c/.h             Tempo (ms) e memória (bytes) de cada algoritmo → logs/resultados.csv (RF03)
│   ├── pesos.c/.h           Distância de Haversine e fator de lentidão por tipo de via
│   ├── geo.c/.h             Vértice mais próximo de uma coordenada (ex.: barragem B1)
│   ├── propagacao.c/.h      Propagação do rejeito por camadas da BFS a partir da B1
│   ├── ponte.c/.h           Pontes topológicas com nome da via, id OSM e tipo (relatório)
│   ├── subgrafos.c/.h       Subgrafos conexos de N vértices via BFS (testes de estresse)
│   ├── cobertura.c/.h       Cobertura de Vértices exata (backtracking com poda)
│   └── main_*.c             Um executável por análise (ver tabela abaixo)
├── tests/
│   └── teste_*.c            Testes de cada módulo (casos de referência + grafo real)
└── saida/                   Relatórios gerados pelos executáveis (CSV/TXT)
```

`bin/` (executáveis) e `logs/` são gerados na compilação/execução e não são versionados.

## Como compilar e executar

Requisitos: GCC e `make` (no Windows, MinGW com `mingw32-make`). Todos os comandos são executados **dentro de `fase2/`**.

```bash
make                        # compila todos os executáveis em bin/
make run T=main_propagacao  # compila e roda bin/main_propagacao com o dataset
make clean                  # apaga bin/
```

| Executável | O que faz | Saída |
|---|---|---|
| `main_propagacao` | Localiza o vértice mais próximo da B1 e simula a frente de rejeitos por camadas da BFS | `saida/afetados.csv` |
| `main_pontes` | Lista as arestas-ponte com metadados da via (nome, OSM, tipo) | `saida/ponte.csv`, `saida/relatorio_pontes.txt` |
| `main_subgrafos` | Gera subgrafos conexos com N = 100, 500 e 1.000 vértices a partir da B1 | `saida/subgrafo_N.txt` |
| `main_cobertura` | Cobertura de Vértices exata sobre um subgrafo | `saida/cobertura_exata.csv` |
| `teste_tipo_via` | Confere a leitura do tipo da via em todas as 3.199 arestas | console |
| `teste_distancia` | Haversine em casos de referência e no grafo real | console |
| `teste_fator_lentidao` | Tabela de lentidão e malha efetiva (km equivalentes) | console |
| `teste_vertice_mais_proximo` | Casos de borda e localização da B1 no grafo | console |
| `teste_propagacao` / `teste_pontes` | Testes em grafos pequenos com resposta conhecida | console |

## Modelagem de pesos

O peso de uma aresta combina três componentes (detalhes em [`tabela_lentidao.md`](tabela_lentidao.md)):

1. **Distância** entre as extremidades pela fórmula de Haversine (`pesos.c`) — implementado.
2. **Fator de lentidão** pelo tipo da via, `fator = 40 km/h ÷ velocidade do tipo` (`tertiary` = 1,00; `track` = 2,67; `path`/`footway` = 8,00) — implementado.
3. **Proximidade da zona de risco** (penalidade para trechos próximos da frente de rejeitos) — *a definir*.

Com as duas primeiras, a malha de 88,3 km equivale a 207,0 km na via de referência (2,34 vezes maior).

## Andamento da Fase II

| Item | Requisito do professor | Situação |
|---|---|---|
| Tipo da via lido do GeoJSON (#45) | Modelagem de pesos | Concluído |
| Distância de Haversine (#46) | Modelagem de pesos | Concluído |
| Tabela de lentidão por tipo de via (#47) | Modelagem de pesos | Concluído |
| Vértice mais próximo da barragem (#48) | Apoio (origem das análises) | Concluído |
| Propagação por camadas a partir da B1 (#49) | Apoio (zona de risco) | Concluído |
| Pontes com metadados das vias (#51) | Apoio (rotas de fuga críticas) | Concluído |
| Subgrafos de 100/500/1.000 vértices (#52) | Testes de estresse | Concluído |
| Cobertura de Vértices exata (#53) | Otimização NP-difícil (exata) | Implementado; `main_cobertura` ainda não lê o formato `saida/subgrafo_N.txt` (chama o leitor de GeoJSON) |
| Componente de risco no peso | Modelagem de pesos | Pendente |
| Dijkstra (rotas seguras) | Otimização P | Pendente |
| Heurística gulosa para a Cobertura | Otimização NP-difícil (heurística) + comparação com a exata | Pendente |
| Matriz de adjacência na Fase II | RF02 (alternar lista × matriz) | Pendente (a estrutura da Fase II mantém só a lista) |
| Testes de estresse e gráficos da Fase II | Protocolo experimental | Pendente |
| Artigo SBC da Fase II | Documentação | Versão com o que já está implementado em [`docs/artigo/fase2/`](../docs/artigo/fase2/) |

## Contrato da Fase II (issue #44) e o que diverge hoje

| Combinado no kickoff | Situação no código |
|---|---|
| `double peso_aresta(Grafo *g, int u, int v)` em `pesos.h` | Ainda não existe; `pesos.h` tem `distancia_entre_vertices` e `fator_lentidao` |
| `void definir_origem_risco(int vertice)` em `pesos.h` | Ainda não existe |
| `int vertice_mais_proximo(Grafo *g, double lat, double lon)` em `geo.h` | Implementado |
| `saida/afetados.csv` → `vertice,lon,lat,camada` | Gerado pelo `main_propagacao` |
| `saida/rota.csv` → `ordem,vertice,lon,lat` | Falta o Dijkstra |
| `saida/pontes.csv` → `vertice_u,vertice_v,lon_u,lat_u,lon_v,lat_v` | Gerado como `saida/ponte.csv` (singular), com colunas `id_origem,id_destino,lon_origem,...` e metadados da via |
| `saida/postos_gulosa.csv` → `vertice,lon,lat` | Falta a heurística gulosa |
| `saida/postos_exata.csv` → `vertice,lon,lat` | Gerado como `saida/cobertura_exata.csv`, colunas `id_vertice,lat,lon` (lat antes de lon) |

Quem for montar a visualização com esses CSVs precisa alinhar os nomes e a ordem das colunas, ou o grupo atualiza o contrato.

## Números de referência (dataset completo)

| Medida | Valor |
|---|---|
| Vértices / arestas | 3.143 / 3.199 |
| Extensão da malha | 88,337 km (trecho médio 27,6 m; menor 1,98 m; maior 711,6 m) |
| Malha efetiva com o fator de lentidão | 207,048 km equivalentes (×2,34) |
| Vértice de origem (mais próximo da B1) | 832, a 125,1 m da barragem |
| Propagação a partir da B1 | 3.115 de 3.143 vértices atingidos, 257 camadas (0 a 256) |
| Arestas-ponte | 1.615 (validadas na Fase I). O `saida/relatorio_pontes.txt` versionado está desatualizado (resumo com 1.601) e deve ser regerado com `make run T=main_pontes` |
| Subgrafos gerados | 100 v / 100 a · 500 v / 509 a · 1.000 v / 1.018 a |
