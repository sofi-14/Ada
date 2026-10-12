#!/usr/bin/env python3
# Programa independiente de Bubble Sort, con generación de datos, medición y CSV.
import csv
import os
import random
import time
from pathlib import Path

SIZES = [100_000, 1_000_000, 10_000_000, 100_000_000, 1_000_000_000]
DEFAULT_MAX_N = 100_000
BUBBLE_LIMIT = 10_000
SEED = 20261011

# Bubble Sort: compara parejas vecinas e intercambia si están al revés.
def bubble_sort(a):
    for fin in range(len(a), 1, -1):
        hubo_cambio = False  # Permite terminar si una pasada no modifica nada.
        for i in range(1, fin):
            if a[i - 1] > a[i]:
                a[i - 1], a[i] = a[i], a[i - 1]
                hubo_cambio = True
        if not hubo_cambio:
            break

def esta_ordenado(datos):
    # Comprueba que cada elemento sea menor o igual que el siguiente.
    return all(datos[i - 1] <= datos[i] for i in range(1, len(datos)))

def main():
    max_n = int(os.environ.get("MAX_N", DEFAULT_MAX_N))
    Path("resultados").mkdir(exist_ok=True)
    ruta = Path("resultados/RESULTADO_BUBBLE_SORT_PYTHON.csv")
    random.seed(SEED)
    with ruta.open("w", newline="", encoding="utf-8") as archivo:
        escritor = csv.writer(archivo)
        escritor.writerow(["algoritmo", "n", "tiempo_s", "memoria_lista_estimada_bytes", "estado", "ordenado"])
        for n in SIZES:
            # Los cinco tamaños se recorren desde el inicio; MAX_N evita reservar memoria sin control.
            if n > max_n:
                escritor.writerow(["Bubble Sort", n, "", "", "OMITIDO_POR_MAX_N", ""])
                continue
            if "Bubble Sort" == "Bubble Sort" and n > BUBBLE_LIMIT:
                escritor.writerow(["Bubble Sort", n, "", "", "OMITIDO_POR_COMPLEJIDAD", ""])
                continue
            try:
                datos = [random.uniform(-1000.0, 1000.0) for _ in range(n)]
            except MemoryError:
                escritor.writerow(["Bubble Sort", n, "", "", "SIN_MEMORIA", ""])
                continue
            inicio = time.perf_counter()
            try:
                bubble_sort(datos)
            except (MemoryError, ValueError) as error:
                escritor.writerow(["Bubble Sort", n, "", "", "NO_COMPLETADO_" + type(error).__name__, "NO"])
                continue
            duracion = time.perf_counter() - inicio
            ok = esta_ordenado(datos)
            # Estimación del contenedor lista + objetos float; no es el pico total del proceso.
            memoria = datos.__sizeof__() + n * (0.0).__sizeof__()
            escritor.writerow(["Bubble Sort", n, f"{duracion:.6f}", memoria,
                               "OK" if ok else "ERROR_O_INCOMPLETO", "SI" if ok else "NO"])
    print(f"Resultados guardados en {ruta}")

if __name__ == "__main__":
    main()
