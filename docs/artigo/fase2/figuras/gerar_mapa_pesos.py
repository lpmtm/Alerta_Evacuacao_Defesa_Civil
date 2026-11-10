"""Gera a Figura 1 do artigo da Fase II: malha viária colorida pelo fator de lentidão.

Uso (a partir da raiz do repositório):
    python docs/artigo/fase2/figuras/gerar_mapa_pesos.py

Lê fase2/data/mapa_brumadinho.geojson e grava mapa_pesos.pdf e mapa_pesos.png
nesta mesma pasta. Os fatores são os de fase2/tabela_lentidao.md
(velocidade de referência 40 km/h ÷ velocidade do tipo).
"""
import json
import math
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D

AQUI = Path(__file__).resolve().parent
RAIZ = AQUI.parents[3]
GEOJSON = RAIZ / "fase2" / "data" / "mapa_brumadinho.geojson"

VELOCIDADE_KMH = {
    "tertiary": 40, "tertiary_link": 30, "residential": 30, "unclassified": 30,
    "service": 20, "track": 15, "path": 5, "footway": 5,
}
VELOCIDADE_PADRAO = 20  # tipo desconhecido (mesmo valor de pesos.c)

# Classes da legenda: (rótulo, fator mínimo, fator máximo, cor, espessura)
CLASSES = [
    ("fator 1,00 (tertiary)", 1.00, 1.00, "#1b5e9e", 1.6),
    ("fator 1,33 (unclassified, residential)", 1.01, 1.50, "#4aa3a2", 1.3),
    ("fator 2,00–2,67 (service, track)", 1.51, 3.00, "#d08c2e", 1.0),
    ("fator 8,00 (path, footway)", 3.01, 99.0, "#b03a2e", 0.8),
]

BARRAGEM_LAT, BARRAGEM_LON = -20.1197, -44.1214  # mesmo ponto de fase2/src/geo.h


def classe(fator):
    for c in CLASSES:
        if c[1] - 1e-9 <= fator <= c[2] + 1e-9:
            return c
    return CLASSES[-1]


def main():
    dados = json.loads(GEOJSON.read_text(encoding="utf-8"))
    lat_media = math.radians(BARRAGEM_LAT)

    fig, ax = plt.subplots(figsize=(6.4, 5.2))
    # Desenha das vias lentas para as rápidas, para as principais ficarem por cima.
    feicoes = sorted(
        dados["features"],
        key=lambda f: -40 / VELOCIDADE_KMH.get(f["properties"].get("highway"), VELOCIDADE_PADRAO),
    )
    for f in feicoes:
        if f["geometry"]["type"] != "LineString":
            continue
        tipo = f["properties"].get("highway")
        fator = 40 / VELOCIDADE_KMH.get(tipo, VELOCIDADE_PADRAO)
        _, _, _, cor, esp = classe(fator)
        xs = [p[0] for p in f["geometry"]["coordinates"]]
        ys = [p[1] for p in f["geometry"]["coordinates"]]
        ax.plot(xs, ys, color=cor, linewidth=esp, solid_capstyle="round")

    ax.plot(BARRAGEM_LON, BARRAGEM_LAT, marker="^", markersize=9, color="black", linestyle="none")
    ax.annotate("Barragem B1", (BARRAGEM_LON, BARRAGEM_LAT), xytext=(-58, 6),
                textcoords="offset points", fontsize=8)

    ax.set_aspect(1 / math.cos(lat_media))
    ax.set_xlabel("Longitude", fontsize=8)
    ax.set_ylabel("Latitude", fontsize=8)
    ax.tick_params(labelsize=7)
    legenda = [Line2D([0], [0], color=c[3], linewidth=2.2, label=c[0]) for c in CLASSES]
    legenda.append(Line2D([0], [0], marker="^", color="black", linestyle="none", label="Barragem B1"))
    ax.legend(handles=legenda, fontsize=7, loc="upper center", bbox_to_anchor=(0.5, -0.12),
              ncol=2, frameon=False)
    fig.tight_layout()

    for ext in ("pdf", "png"):
        fig.savefig(AQUI / f"mapa_pesos.{ext}", dpi=300, bbox_inches="tight")
    print("Figura gravada em", AQUI)


if __name__ == "__main__":
    main()
