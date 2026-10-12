#!/usr/bin/env python3
# Programa independiente de Tree Sort, con generación de datos, medición y CSV.
import csv
import os
import random
import time
from pathlib import Path

SIZES = [100_000, 1_000_000, 10_000_000, 100_000_000, 1_000_000_000]
DEFAULT_MAX_N = 100_000
BUBBLE_LIMIT = 10_000
SEED = 20261011

# Tree Sort: inserta cada valor en un árbol binario de búsqueda.
class Nodo:
    __slots__ = ("valor", "izq", "der")
    def __init__(self, valor):
        self.valor = valor
        self.izq = None
        self.der = None

def tree_sort(a):
    if len(a) < 2:
        return
    raiz = Nodo(a[0])
    for valor in a[1:]:
        actual = raiz
        while True:
            if valor < actual.valor:
                if actual.izq is None:
                    actual.izq = Nodo(valor); break
                actual = actual.izq
            else:
                if actual.der is None:
                    actual.der = Nodo(valor); break
                actual = actual.der

    # El recorrido inorden visita izquierda, nodo y derecha: produce orden ascendente.
    pila, salida, actual = [], [], raiz
    while actual is not None or pila:
        while actual is not None:
            pila.append(actual)
            actual = actual.izq
        actual = pila.pop()
        salida.append(actual.valor)
        actual = actual.der
    a[:] = salida

def esta_ordenado(datos):
    # Comprueba que cada elemento sea menor o igual que el siguiente.
    return all(datos[i - 1] <= datos[i] for i in range(1, len(datos)))

def main():
    max_n = int(os.environ.get("MAX_N", DEFAULT_MAX_N))
    Path("resultados").mkdir(exist_ok=True)
    ruta = Path("resultados/RESULTADO_TREE_SORT_PYTHON.csv")
    random.seed(SEED)
    with ruta.open("w", newline="", encoding="utf-8") as archivo:
        escritor = csv.writer(archivo)
        escritor.writerow(["algoritmo", "n", "tiempo_s", "memoria_lista_estimada_bytes", "estado", "ordenado"])
        for n in SIZES:
            # Los cinco tamaños se recorren desde el inicio; MAX_N evita reservar memoria sin control.
            if n > max_n:
                escritor.writerow(["Tree Sort", n, "", "", "OMITIDO_POR_MAX_N", ""])
                continue
            if "Tree Sort" == "Bubble Sort" and n > BUBBLE_LIMIT:
                escritor.writerow(["Tree Sort", n, "", "", "OMITIDO_POR_COMPLEJIDAD", ""])
                continue
            try:
                datos = [random.uniform(-1000.0, 1000.0) for _ in range(n)]
            except MemoryError:
                escritor.writerow(["Tree Sort", n, "", "", "SIN_MEMORIA", ""])
                continue
            inicio = time.perf_counter()
            try:
                tree_sort(datos)
            except (MemoryError, ValueError) as error:
                escritor.writerow(["Tree Sort", n, "", "", "NO_COMPLETADO_" + type(error).__name__, "NO"])
                continue
            duracion = time.perf_counter() - inicio
            ok = esta_ordenado(datos)
            # Estimación del contenedor lista + objetos float; no es el pico total del proceso.
            memoria = datos.__sizeof__() + n * (0.0).__sizeof__()
            escritor.writerow(["Tree Sort", n, f"{duracion:.6f}", memoria,
                               "OK" if ok else "ERROR_O_INCOMPLETO", "SI" if ok else "NO"])
    print(f"Resultados guardados en {ruta}")

if __name__ == "__main__":
    main()
