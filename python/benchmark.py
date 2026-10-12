"""
Proyecto: comparación de algoritmos de ordenamiento en Python.
Incluye los cinco tamaños requeridos desde el inicio.
Ejecutar: python python/benchmark.py
Control de memoria/tiempo: MAX_N=1000000 python python/benchmark.py
Resultados: resultados/resultados_Python.csv
"""

import csv
import math
import os
import random
import time
import tracemalloc
import sys
from pathlib import Path

SIZES = [100_000, 1_000_000, 10_000_000, 100_000_000, 1_000_000_000]
DEFAULT_MAX_N = 1_000_000
BUBBLE_LIMIT = 10_000
COUNTING_MAX_RANGE = 5_000_000
SEED = 20261011
OUTPUT = Path("resultados/resultados_Python.csv")


# Bubble Sort: compara pares vecinos; cada pasada desplaza el mayor al final.
def bubble_sort(a):
    for end in range(len(a), 1, -1):
        changed = False
        for i in range(1, end):
            if a[i - 1] > a[i]:
                a[i - 1], a[i] = a[i], a[i - 1]
                changed = True
        if not changed:
            break


# Merge Sort: divide el segmento, ordena ambas mitades y después las combina.
def merge_sort(a):
    def merge_range(lo, hi):
        if hi - lo < 2:
            return
        mid = (lo + hi) // 2
        merge_range(lo, mid)
        merge_range(mid, hi)
        left = a[lo:mid]
        right = a[mid:hi]
        i = j = 0
        merged = []
        while i < len(left) and j < len(right):
            if left[i] <= right[j]:
                merged.append(left[i])
                i += 1
            else:
                merged.append(right[j])
                j += 1
        merged.extend(left[i:])
        merged.extend(right[j:])
        a[lo:hi] = merged
    merge_range(0, len(a))


# Heap Sort: crea un montículo máximo y extrae el mayor elemento en cada vuelta.
def heap_sort(a):
    n = len(a)

    def sift_down(root, end):
        while True:
            child = 2 * root + 1
            if child >= end:
                return
            if child + 1 < end and a[child] < a[child + 1]:
                child += 1
            if a[root] >= a[child]:
                return
            a[root], a[child] = a[child], a[root]
            root = child

    for i in range(n // 2 - 1, -1, -1):
        sift_down(i, n)
    for end in range(n - 1, 0, -1):
        a[0], a[end] = a[end], a[0]
        sift_down(0, end)


# Tree Sort: inserta datos en un árbol binario de búsqueda y recorre en inorden.
# La implementación es iterativa para no depender del límite de recursión de Python.
class Node:
    __slots__ = ("value", "left", "right")

    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None


def tree_sort(a):
    if len(a) < 2:
        return
    root = Node(a[0])
    for value in a[1:]:
        current = root
        while True:
            if value < current.value:
                if current.left is None:
                    current.left = Node(value)
                    break
                current = current.left
            else:
                if current.right is None:
                    current.right = Node(value)
                    break
                current = current.right

    stack = []
    output = []
    current = root
    while current is not None or stack:
        while current is not None:
            stack.append(current)
            current = current.left
        current = stack.pop()
        output.append(current.value)
        current = current.right
    a[:] = output


# Counting Sort: discretiza a centésimas (x*100), cuenta claves y reconstruye.
# Por ello, valores con más de dos decimales pueden cambiar.
def counting_sort(a):
    if not a:
        return
    keys = [round(value * 100) for value in a]
    min_key = min(keys)
    max_key = max(keys)
    span = max_key - min_key + 1
    if span > COUNTING_MAX_RANGE:
        raise ValueError(
            f"rango de claves {span} excede el límite {COUNTING_MAX_RANGE}"
        )
    counts = [0] * span
    for key in keys:
        counts[key - min_key] += 1
    output = []
    for offset, count in enumerate(counts):
        if count:
            output.extend([(offset + min_key) / 100.0] * count)
    a[:] = output


ALGORITHMS = [
    ("BubbleSort", bubble_sort),
    ("MergeSort", merge_sort),
    ("TreeSort", tree_sort),
    ("HeapSort", heap_sort),
    ("CountingSort", counting_sort),
]


def is_sorted(a):
    """Devuelve True si cada elemento es menor o igual que el siguiente."""
    return all(a[i - 1] <= a[i] for i in range(1, len(a)))


def get_max_n():
    """Lee MAX_N del entorno; el valor predeterminado evita asignaciones enormes."""
    try:
        return max(1, int(os.environ.get("MAX_N", DEFAULT_MAX_N)))
    except ValueError:
        return DEFAULT_MAX_N


def main():
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    random.seed(SEED)
    max_n = get_max_n()

    with OUTPUT.open("w", newline="", encoding="utf-8") as file:
        writer = csv.writer(file)
        writer.writerow([
            "lenguaje", "algoritmo", "n", "tiempo_s",
            "memoria_pico_bytes", "memoria_arreglo_estimado_bytes", "estado", "ordenado"
        ])

        for n in SIZES:
            # Todos los tamaños se recorren desde el inicio; los superiores a MAX_N
            # quedan documentados como omitidos y no se asignan de manera peligrosa.
            if n > max_n:
                for name, _ in ALGORITHMS:
                    writer.writerow([
                        "Python", name, n, "", "", "", "OMITIDO_POR_LIMITE_MAX_N", ""
                    ])
                continue

            try:
                base = [random.uniform(-1000.0, 1000.0) for _ in range(n)]
            except MemoryError:
                for name, _ in ALGORITHMS:
                    writer.writerow([
                        "Python", name, n, "", "", "", "SIN_MEMORIA_PARA_ARREGLO", ""
                    ])
                continue

            for name, function in ALGORITHMS:
                if name == "BubbleSort" and n > BUBBLE_LIMIT:
                    writer.writerow([
                        "Python", name, n, "", "", "", "OMITIDO_POR_COMPLEJIDAD", ""
                    ])
                    continue

                try:
                    data = base.copy()
                except MemoryError:
                    writer.writerow([
                        "Python", name, n, "", "", "", "SIN_MEMORIA_PARA_COPIA", ""
                    ])
                    continue

                start = time.perf_counter()
                try:
                    # El tiempo se mide sin tracemalloc para no distorsionar el cronómetro.
                    function(data)
                    elapsed = time.perf_counter() - start
                    valid = is_sorted(data)

                    # tracemalloc es costoso; se mide memoria pico en tamaños pequeños
                    # por separado. Para tamaños mayores se deja vacío y se explica
                    # que debe medirse con un monitor de proceso del sistema operativo.
                    peak_text = ""
                    # Estimación del contenedor de entrada: lista + objetos float.
                    # No representa el pico de memoria de todo el algoritmo.
                    array_estimate = sys.getsizeof(base) + n * sys.getsizeof(0.0)
                    if n <= 10_000:
                        sample = base.copy()
                        tracemalloc.start()
                        try:
                            function(sample)
                            _, peak = tracemalloc.get_traced_memory()
                            peak_text = peak
                        finally:
                            tracemalloc.stop()

                    writer.writerow([
                        "Python", name, n, f"{elapsed:.6f}", peak_text, array_estimate,
                        "OK" if valid else "ERROR_O_INCOMPLETO",
                        "SI" if valid else "NO"
                    ])
                except (MemoryError, ValueError, OverflowError) as error:
                    writer.writerow([
                        "Python", name, n, "", "", "",
                        f"NO_COMPLETADO_{type(error).__name__}", "NO"
                    ])
            file.flush()

    print(f"Resultados guardados en {OUTPUT}")
    print(f"Los cinco tamaños están definidos. MAX_N actual: {max_n}")


if __name__ == "__main__":
    main()
