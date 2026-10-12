#!/usr/bin/env python3
# Programa independiente de Counting Sort, con generación de datos, medición y CSV.
import csv
import os
import random
import time
from pathlib import Path

SIZES = [100_000, 1_000_000, 10_000_000, 100_000_000, 1_000_000_000]
DEFAULT_MAX_N = 100_000
BUBBLE_LIMIT = 10_000
SEED = 20261011

# Counting Sort necesita claves discretas; aquí cuantiza a centésimas.
def counting_sort(a):
    if not a:
        return
    claves = [round(valor * 100) for valor in a]
    minimo, maximo = min(claves), max(claves)
    rango = maximo - minimo + 1
    if rango > 5_000_000:
        raise ValueError("El rango de claves es demasiado grande para contar")
    cuentas = [0] * rango
    for clave in claves:
        cuentas[clave - minimo] += 1  # Registra la frecuencia de cada clave.

    salida = []
    for desplazamiento, cantidad in enumerate(cuentas):
        if cantidad:
            valor = (desplazamiento + minimo) / 100
            salida.extend([valor] * cantidad)
    a[:] = salida

def esta_ordenado(datos):
    # Comprueba que cada elemento sea menor o igual que el siguiente.
    return all(datos[i - 1] <= datos[i] for i in range(1, len(datos)))

def main():
    max_n = int(os.environ.get("MAX_N", DEFAULT_MAX_N))
    Path("resultados").mkdir(exist_ok=True)
    ruta = Path("resultados/RESULTADO_COUNTING_SORT_PYTHON.csv")
    random.seed(SEED)
    with ruta.open("w", newline="", encoding="utf-8") as archivo:
        escritor = csv.writer(archivo)
        escritor.writerow(["algoritmo", "n", "tiempo_s", "memoria_lista_estimada_bytes", "estado", "ordenado"])
        for n in SIZES:
            # Los cinco tamaños se recorren desde el inicio; MAX_N evita reservar memoria sin control.
            if n > max_n:
                escritor.writerow(["Counting Sort", n, "", "", "OMITIDO_POR_MAX_N", ""])
                continue
            if "Counting Sort" == "Bubble Sort" and n > BUBBLE_LIMIT:
                escritor.writerow(["Counting Sort", n, "", "", "OMITIDO_POR_COMPLEJIDAD", ""])
                continue
            try:
                datos = [random.uniform(-1000.0, 1000.0) for _ in range(n)]
            except MemoryError:
                escritor.writerow(["Counting Sort", n, "", "", "SIN_MEMORIA", ""])
                continue
            inicio = time.perf_counter()
            try:
                counting_sort(datos)
            except (MemoryError, ValueError) as error:
                escritor.writerow(["Counting Sort", n, "", "", "NO_COMPLETADO_" + type(error).__name__, "NO"])
                continue
            duracion = time.perf_counter() - inicio
            ok = esta_ordenado(datos)
            # Estimación del contenedor lista + objetos float; no es el pico total del proceso.
            memoria = datos.__sizeof__() + n * (0.0).__sizeof__()
            escritor.writerow(["Counting Sort", n, f"{duracion:.6f}", memoria,
                               "OK" if ok else "ERROR_O_INCOMPLETO", "SI" if ok else "NO"])
    print(f"Resultados guardados en {ruta}")

if __name__ == "__main__":
    main()
