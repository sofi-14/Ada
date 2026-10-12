"""Genera gráficas de tiempo y memoria desde los CSV de C y Python."""
import csv
import sys
from pathlib import Path
import matplotlib.pyplot as plt

def read_rows(paths):
    rows = []
    for path in paths:
        if Path(path).exists():
            with open(path, newline="", encoding="utf-8") as file:
                rows.extend(csv.DictReader(file))
    return rows

def main():
    paths = sys.argv[1:] or [
        "resultados/resultados_C.csv",
        "resultados/resultados_Python.csv"
    ]
    rows = read_rows(paths)
    if not rows:
        print("No hay CSV todavía. Ejecuta primero los benchmarks.")
        return
    out = Path("resultados")
    out.mkdir(exist_ok=True)

    charts = [
        ("tiempo_s", "Tiempo de ejecución (segundos)", "comparacion_tiempo.png"),
        (None, "Memoria medida / bytes del arreglo (bytes)", "comparacion_memoria.png")
    ]
    for column, ylabel, filename in charts:
        plt.figure(figsize=(10, 6))
        labels = sorted({(r["lenguaje"], r["algoritmo"]) for r in rows})
        for language, algorithm in labels:
            points = []
            for row in rows:
                if row["lenguaje"] != language or row["algoritmo"] != algorithm:
                    continue
                if not row.get("estado", "").startswith("OK"):
                    continue
                value = row.get(column, "") if column else (
                    row.get("memoria_pico_bytes") or row.get("memoria_arreglo_bytes") or row.get("memoria_arreglo_estimado_bytes") or ""
                )
                try:
                    x, y = int(row["n"]), float(value)
                    if y > 0:
                        points.append((x, y))
                except (ValueError, TypeError):
                    pass
            points.sort()
            if points:
                plt.plot([p[0] for p in points], [p[1] for p in points],
                         marker="o", label=f"{language} - {algorithm}")
        plt.xscale("log")
        plt.yscale("log")
        plt.xlabel("Número de elementos (escala logarítmica)")
        plt.ylabel(ylabel)
        plt.title("Comparación de algoritmos de ordenamiento")
        plt.grid(True, which="both", alpha=0.3)
        plt.legend(fontsize="small")
        plt.tight_layout()
        plt.savefig(out / filename, dpi=180)
        plt.close()
    print("Gráficas guardadas en resultados/")

if __name__ == "__main__":
    main()
