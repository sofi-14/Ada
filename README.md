# Comparación de algoritmos de ordenamiento en C y Python

Proyecto para comparar cinco algoritmos de ordenamiento, medir tiempo de ejecución, registrar una medida de memoria disponible y validar que los resultados queden ordenados.

## Algoritmos incluidos

- Bubble Sort
- Merge Sort
- Tree Sort (árbol binario de búsqueda no balanceado)
- Heap Sort
- Counting Sort

Cada algoritmo está implementado en C y Python. Los archivos individuales contienen el código completo de cada algoritmo; los programas `benchmark` ejecutan la batería de pruebas y guardan resultados en CSV.

## Tamaños de entrada solicitados

**El tamaño de 100,000 elementos SÍ está incluido** como la primera prueba en el reporte, en cada programa individual de C y Python, en ambos benchmarks y en los CSV. No se debe confundir con 100,000,000. Los programas recorren estos cinco tamaños, en este orden:

1. `100000` (100,000)
2. `1000000` (1,000,000)
3. `10000000` (10,000,000)
4. `100000000` (100,000,000)
5. `1000000000` (1,000,000,000)

**Importante:** que un tamaño esté configurado no significa que se haya medido. Los tamaños que superen `MAX_N` se registran como omitidos por el límite. Si no hay memoria suficiente, el programa registra el estado correspondiente. No se deben presentar valores omitidos como tiempos medidos.

## Estructura del repositorio

```text
ordenamientos_entrega/
├── README.md
├── .gitignore
├── graficar.py
├── c/
│   ├── benchmark.c
│   ├── bubble_sort.c
│   ├── merge_sort.c
│   ├── tree_sort.c
│   ├── heap_sort.c
│   └── counting_sort.c
├── python/
│   ├── benchmark.py
│   ├── bubble_sort.py
│   ├── merge_sort.py
│   ├── tree_sort.py
│   ├── heap_sort.py
│   └── counting_sort.py
└── resultados/
    ├── resultados_C.csv
    ├── resultados_Python.csv
    └── gráficas PNG
```

## Ejecutar las pruebas

### C (Linux/macOS)

```bash
gcc -O2 -std=c11 c/benchmark.c -o benchmark_c -lm
./benchmark_c
```

El límite predeterminado de C es `10,000,000` elementos. Para cambiarlo en una terminal compatible con POSIX:

```bash
MAX_N=100000000 ./benchmark_c
```

En Windows PowerShell, después de compilar `benchmark.exe`:

```powershell
$env:MAX_N = "100000000"
.\benchmark.exe
```

### Python

Instala Python 3.10 o posterior. Ejecuta:

```bash
python python/benchmark.py
```

El límite predeterminado de Python es `1,000,000` elementos. Para cambiarlo en Linux/macOS:

```bash
MAX_N=10000000 python python/benchmark.py
```

En Windows PowerShell:

```powershell
$env:MAX_N = "10000000"
python python/benchmark.py
```

Aumenta `MAX_N` gradualmente y solo si el equipo tiene memoria suficiente. Python requiere considerablemente más memoria que un arreglo equivalente de `double` en C.

## Límites importantes de interpretación

- **Bubble Sort:** se omite por encima de 10,000 elementos debido a su complejidad cuadrática `O(n²)`. No es razonable esperar que termine en tiempos útiles para tamaños de millones o más.
- **Tree Sort:** usa un árbol binario no balanceado; puede degradarse a `O(n²)` y consumir mucha memoria.
- **Counting Sort:** para estos datos reales, la implementación convierte valores a claves en centésimas. Los valores con más de dos decimales pueden cambiar. Además, existe un límite de rango de claves para evitar una asignación excesiva.
- **Memoria en C:** el CSV registra el tamaño del arreglo principal (`n * sizeof(double)`), no el máximo de memoria de todo el proceso.
- **Memoria en Python:** se registra una estimación del contenedor de entrada. La medición de pico con `tracemalloc` se realiza solo en arreglos pequeños para evitar alterar mucho las pruebas.
- Los resultados dependen del procesador, RAM, sistema operativo, compilador, versión de Python y carga del equipo. Las pruebas grandes deben ejecutarse en el equipo que se reportará y no extrapolarse a partir de las pequeñas.

## Resultados y gráficas

Los CSV incluidos contienen filas para **100,000 elementos para los cinco algoritmos en ambos lenguajes**. Bubble Sort aparece con estado `OMITIDO_POR_COMPLEJIDAD` porque 100,000 elementos requieren miles de millones de comparaciones; no se inventa un tiempo para esa prueba. Merge, Tree, Heap y Counting tienen tiempos medidos para 100,000 elementos en este entorno.

Los benchmarks generan:

- `resultados/resultados_C.csv`
- `resultados/resultados_Python.csv`

Después de ejecutar ambos benchmarks, instala Matplotlib y genera las gráficas:

```bash
python -m pip install matplotlib
python graficar.py
```

Las gráficas se guardan en `resultados/`. Las imágenes ya incluidas en esa carpeta corresponden a resultados de pruebas realizadas anteriormente; al ejecutar nuevos benchmarks, vuelve a generar las gráficas para que reflejen tus nuevos CSV.

## Subir a GitHub

Desde la carpeta raíz del proyecto:

```bash
git init
git add .
git commit -m "Implementación y comparación de algoritmos en C y Python"
git branch -M main
git remote add origin URL_DE_TU_REPOSITORIO
git push -u origin main
```

Sustituye `URL_DE_TU_REPOSITORIO` por la URL del repositorio que creaste en GitHub.
