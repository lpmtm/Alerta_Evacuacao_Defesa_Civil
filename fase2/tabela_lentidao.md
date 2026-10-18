# Tabela de lentidão por tipo de via

Documento de apoio à seção **Modelagem de Pesos** do artigo (Fase 2). Implementação em `fase2/src/pesos.c`.

## 1. Objetivo

O peso de cada aresta do grafo será composto por três componentes: distância, tipo da via e proximidade da zona de risco. Este documento define a segunda componente: um **fator de lentidão** por tipo de via, que traduz o fato de que percorrer 100 m numa trilha de terra leva mais tempo do que percorrer 100 m numa via asfaltada.

## 2. Definição do fator

O fator é o tempo de percurso de uma via em relação a uma via de referência, para o mesmo comprimento:

```
fator = velocidade_de_referencia / velocidade_do_tipo
```

- **Velocidade de referência: 40 km/h**, correspondente ao tipo `tertiary` (fator 1,00).
- Fator 2,00 significa que o mesmo trecho leva o dobro do tempo; fator 8,00, oito vezes mais.
- Na composição do peso, `distância × fator` equivale a "metros equivalentes na via de referência".

O fator mede **lentidão**. A **capacidade** (número de faixas, fluxo de veículos) não foi modelada porque o OpenStreetMap não traz esse dado de forma confiável para a região.

## 3. Tabela

| Tipo (OSM) | Velocidade assumida | Fator | Base da escolha |
|---|---|---|---|
| tertiary | 40 km/h | 1,00 | Equivale à via coletora do CTB (40 km/h) |
| tertiary_link | 30 km/h | 1,33 | Ramal de ligação; hipótese: mais lento que a via principal |
| residential | 30 km/h | 1,33 | Equivale à via local do CTB (30 km/h) |
| unclassified | 30 km/h | 1,33 | Via pública menor, de menor importância que a tertiary; tratada como via local |
| service | 20 km/h | 2,00 | Hipótese: acessos curtos, com manobras e circulação junto a propriedades |
| track | 15 km/h | 2,67 | Hipótese: estrada rural em geral não pavimentada |
| path | 5 km/h | 8,00 | Hipótese: passagem a pé, não destinada a veículos |
| footway | 5 km/h | 8,00 | Hipótese: passagem a pé, não destinada a veículos |
| outro / desconhecida | 20 km/h | 2,00 | Valor conservador para tipos não previstos ou sem etiqueta |

## 4. Justificativa

**Vias pavimentadas (tertiary, residential, unclassified, tertiary_link).** Foram ancoradas no art. 61, § 1º, do Código de Trânsito Brasileiro, que fixa, na ausência de sinalização, 30 km/h para vias locais, 40 km/h para coletoras e 60 km/h para arteriais. Segundo o guia de classificação do OpenStreetMap, as vias `tertiary` reúnem o tráfego de vias menores e o levam às principais, papel equivalente ao da via coletora. Já `unclassified` designa vias públicas menores, utilizáveis por carros e de menor importância que a `tertiary`.

**Vias de acesso e estradas rurais (service, track).** Para `service`, o OpenStreetMap descreve vias que dão o acesso final a áreas e instalações (como entradas de propriedades), raramente longas e contínuas. Para `track`, descreve estradas rurais na maior parte das vezes não pavimentadas, voltadas ao acesso a terrenos agrícolas ou florestais, e que garantem apenas largura para um veículo de quatro rodas. O CTB fixa 60 km/h para estradas, mas esse valor é um **limite legal máximo**, não uma velocidade típica. Em um cenário de emergência, com chão irregular, possível lama e trechos estreitos, adotamos 15 km/h para `track` e 20 km/h para `service`.

**Passagens a pé (path, footway).** São tratadas como caminhada (5 km/h). O fator alto, mas **finito**, mantém essas ligações no grafo: servem à fuga a pé, e o Dijkstra da ambulância as evitará sempre que houver alternativa.

## 5. Impacto na malha de Córrego do Feijão

| Tipo | Arestas | km | % da malha | Fator | km equivalentes |
|---|---|---|---|---|---|
| service | 818 | 24,684 | 27,9% | 2,00 | 49,368 |
| track | 726 | 23,038 | 26,1% | 2,67 | 61,436 |
| unclassified | 811 | 19,153 | 21,7% | 1,33 | 25,537 |
| tertiary | 521 | 11,399 | 12,9% | 1,00 | 11,399 |
| path | 190 | 6,321 | 7,2% | 8,00 | 50,565 |
| residential | 101 | 3,156 | 3,6% | 1,33 | 4,208 |
| footway | 31 | 0,563 | 0,6% | 8,00 | 4,505 |
| tertiary_link | 1 | 0,022 | 0,0% | 1,33 | 0,029 |
| **Total** | **3199** | **88,337** | | | **207,048** |

- Apenas 12,9% da malha é via de referência; **61,8%** é `track`, `service`, `path` ou `footway`.
- A malha efetiva é **2,34 vezes** maior que a malha real: a lentidão pesa muito nas rotas.

## 6. Hipóteses e limitações

- As velocidades de `service`, `track`, `path`, `footway` e `tertiary_link` são **hipóteses do grupo**, não medições. Não houve calibração com dados reais de deslocamento de ambulâncias.
- As velocidades do CTB são limites legais, usados aqui como aproximação da velocidade típica.
- Como a tabela tem grande influência no resultado, recomenda-se uma **análise de sensibilidade** na discussão: repetir a rota da ambulância com tabelas alternativas (por exemplo, `track` a 20 km/h) e verificar se a rota muda.
- Se o grupo preferir proibir `path` e `footway` para ambulâncias, basta tratar fator maior ou igual a 8 como intransitável no Dijkstra.
- A superfície (`surface`) e a largura da via, presentes em parte do arquivo, não foram usadas.

## 7. Como alterar

As velocidades ficam no vetor `VELOCIDADE_VIA_KMH` em `fase2/src/pesos.c`. Os fatores são calculados a partir dele, então basta mudar a velocidade de um tipo para que o fator se ajuste. Depois, rodar `make run T=teste_fator_lentidao`.

## Referências

- BRASIL. Lei nº 9.503/1997 (Código de Trânsito Brasileiro), art. 61, § 1º.
- OpenStreetMap Wiki. *Tag:highway=track*. https://wiki.openstreetmap.org/Tag:highway=track
- OpenStreetMap Wiki. *Tag:highway=unclassified*. https://wiki.openstreetmap.org/wiki/Unclassified
- OpenStreetMap Wiki. *Kaart: Road Classifications Guide*. https://wiki.openstreetmap.org/wiki/Kaart:_Road_Classifications_Guide
