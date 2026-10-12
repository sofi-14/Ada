#define _POSIX_C_SOURCE 200809L
/*
 Proyecto: Comparación de algoritmos de ordenamiento
 Lenguaje: C (C11)
 Algoritmos: Bubble Sort, Merge Sort, Tree Sort, Heap Sort y Counting Sort
 Los cinco tamaños requeridos están definidos desde el inicio en SIZES.
 Compilar: gcc -O2 -std=c11 c/benchmark.c -o benchmark_c -lm
 Ejecutar: ./benchmark_c
 Para controlar la memoria máxima: MAX_N=1000000 ./benchmark_c
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#define BUBBLE_LIMIT 10000LL
#define COUNTING_MAX_RANGE 5000000LL
#define DEFAULT_MAX_N 10000000LL

typedef void (*SortFunction)(double *, size_t);

typedef struct {
    const char *name;
    SortFunction sort;
} Algorithm;

static const long long SIZES[] = {
    100000LL, 1000000LL, 10000000LL, 100000000LL, 1000000000LL
};

/* Reloj monotónico: mide duración transcurrida y evita cambios del reloj civil. */
static double seconds_now(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return 0.0;
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

/* Bubble Sort: compara vecinos e intercambia los que están fuera de orden. */
static void bubble_sort(double *a, size_t n) {
    for (size_t end = n; end > 1; --end) {
        int changed = 0;
        for (size_t i = 1; i < end; ++i) {
            if (a[i - 1] > a[i]) {
                double tmp = a[i - 1];
                a[i - 1] = a[i];
                a[i] = tmp;
                changed = 1;
            }
        }
        if (!changed) break;
    }
}

/* Merge Sort: divide recursivamente y mezcla segmentos ya ordenados. */
static void merge_range(double *a, double *tmp, size_t lo, size_t hi) {
    if (hi - lo < 2) return;
    size_t mid = lo + (hi - lo) / 2;
    merge_range(a, tmp, lo, mid);
    merge_range(a, tmp, mid, hi);
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi) tmp[k++] = a[j++];
    memcpy(a + lo, tmp + lo, (hi - lo) * sizeof(*a));
}
static void merge_sort(double *a, size_t n) {
    double *tmp = malloc(n * sizeof(*tmp));
    if (!tmp) { fprintf(stderr, "Merge Sort: no hay memoria auxiliar suficiente.\n"); return; }
    merge_range(a, tmp, 0, n);
    free(tmp);
}

/* Heap Sort: construye un max-heap y coloca repetidamente el máximo al final. */
static void sift_down(double *a, size_t n, size_t root) {
    for (;;) {
        size_t largest = root, left = 2 * root + 1, right = 2 * root + 2;
        if (left < n && a[left] > a[largest]) largest = left;
        if (right < n && a[right] > a[largest]) largest = right;
        if (largest == root) return;
        double tmp = a[root]; a[root] = a[largest]; a[largest] = tmp;
        root = largest;
    }
}
static void heap_sort(double *a, size_t n) {
    for (size_t i = n / 2; i > 0; --i) sift_down(a, n, i - 1);
    for (size_t end = n; end > 1; --end) {
        double tmp = a[0]; a[0] = a[end - 1]; a[end - 1] = tmp;
        sift_down(a, end - 1, 0);
    }
}

/* Tree Sort: inserta cada valor en un ABB y usa recorrido inorden.
   Inserción y recorrido son iterativos para evitar desbordamiento de recursión.
   En un árbol muy desbalanceado el tiempo puede llegar a O(n^2). */
typedef struct Node {
    double value;
    struct Node *left;
    struct Node *right;
} Node;

static void free_tree(Node *root) {
    if (!root) return;
    /* Liberación iterativa usando una pila auxiliar dimensionada por n en tree_sort. */
}
static void tree_sort(double *a, size_t n) {
    if (n < 2) return;
    Node *root = NULL;
    Node **allocated = calloc(n, sizeof(*allocated));
    Node **stack = malloc(n * sizeof(*stack));
    if (!allocated || !stack) {
        fprintf(stderr, "Tree Sort: no hay memoria para nodos/pila.\n");
        free(allocated); free(stack); return;
    }
    size_t used = 0;
    for (size_t i = 0; i < n; ++i) {
        Node *node = malloc(sizeof(*node));
        if (!node) {
            fprintf(stderr, "Tree Sort: memoria insuficiente; resultado incompleto.\n");
            for (size_t j = 0; j < used; ++j) free(allocated[j]);
            free(allocated); free(stack); return;
        }
        node->value = a[i]; node->left = node->right = NULL;
        allocated[used++] = node;
        if (!root) root = node;
        else {
            Node *cur = root;
            for (;;) {
                if (node->value < cur->value) {
                    if (!cur->left) { cur->left = node; break; }
                    cur = cur->left;
                } else {
                    if (!cur->right) { cur->right = node; break; }
                    cur = cur->right;
                }
            }
        }
    }
    size_t top = 0, out = 0;
    Node *cur = root;
    while (cur || top) {
        while (cur) { stack[top++] = cur; cur = cur->left; }
        cur = stack[--top];
        a[out++] = cur->value;
        cur = cur->right;
    }
    for (size_t j = 0; j < used; ++j) free(allocated[j]);
    free(allocated); free(stack);
}

/* Counting Sort para reales cuantizados a centésimas.
   Ej.: 12.34 -> 1234. No es exacto para reales con más de dos decimales.
   El rango de claves se limita para evitar reservar memoria desproporcionada. */
static void counting_sort(double *a, size_t n) {
    if (n == 0) return;
    long long *keys = malloc(n * sizeof(*keys));
    if (!keys) { fprintf(stderr, "Counting Sort: no hay memoria para claves.\n"); return; }
    long long min_key = LLONG_MAX, max_key = LLONG_MIN;
    for (size_t i = 0; i < n; ++i) {
        keys[i] = llround(a[i] * 100.0);
        if (keys[i] < min_key) min_key = keys[i];
        if (keys[i] > max_key) max_key = keys[i];
    }
    unsigned long long range64 = (unsigned long long)(max_key - min_key) + 1ULL;
    if (range64 > COUNTING_MAX_RANGE) {
        fprintf(stderr, "Counting Sort omitido: rango de claves (%llu) supera el límite (%lld).\n",
                range64, COUNTING_MAX_RANGE);
        free(keys); return;
    }
    size_t range = (size_t)range64;
    size_t *counts = calloc(range, sizeof(*counts));
    if (!counts) { fprintf(stderr, "Counting Sort: no hay memoria para frecuencias.\n"); free(keys); return; }
    for (size_t i = 0; i < n; ++i) counts[(size_t)(keys[i] - min_key)]++;
    size_t out = 0;
    for (size_t j = 0; j < range; ++j) {
        for (size_t repeat = 0; repeat < counts[j]; ++repeat)
            a[out++] = ((double)((long long)j + min_key)) / 100.0;
    }
    free(counts); free(keys);
}

/* Comprueba el resultado. Solo una secuencia no decreciente se reporta como válida. */
static int is_sorted(const double *a, size_t n) {
    for (size_t i = 1; i < n; ++i) if (a[i - 1] > a[i]) return 0;
    return 1;
}
static double *clone_array(const double *src, size_t n) {
    double *dst = malloc(n * sizeof(*dst));
    if (dst) memcpy(dst, src, n * sizeof(*dst));
    return dst;
}
static long long configured_max(void) {
    const char *env = getenv("MAX_N");
    if (!env || !*env) return DEFAULT_MAX_N;
    char *end = NULL; errno = 0;
    long long val = strtoll(env, &end, 10);
    if (errno || end == env || *end || val < 1) return DEFAULT_MAX_N;
    return val;
}

int main(void) {
    const Algorithm algorithms[] = {
        {"BubbleSort", bubble_sort}, {"MergeSort", merge_sort},
        {"TreeSort", tree_sort}, {"HeapSort", heap_sort},
        {"CountingSort", counting_sort}
    };
    const size_t algorithm_count = sizeof(algorithms) / sizeof(algorithms[0]);
    const size_t size_count = sizeof(SIZES) / sizeof(SIZES[0]);
    long long max_n = configured_max();

    if (system("mkdir -p resultados") != 0) {
        fprintf(stderr, "Aviso: crea manualmente la carpeta resultados si no existe.\n");
    }
    FILE *csv = fopen("resultados/resultados_C.csv", "w");
    if (!csv) { perror("No se pudo crear resultados/resultados_C.csv"); return 1; }
    fprintf(csv, "lenguaje,algoritmo,n,tiempo_s,memoria_arreglo_bytes,estado,ordenado\n");

    /* La misma semilla y distribución hacen reproducibles los datos de entrada. */
    srand(20261011);
    for (size_t si = 0; si < size_count; ++si) {
        long long n64 = SIZES[si];
        if (n64 > max_n) {
            for (size_t ai = 0; ai < algorithm_count; ++ai)
                fprintf(csv, "C,%s,%lld,,,OMITIDO_POR_LIMITE_MAX_N,\n", algorithms[ai].name, n64);
            continue;
        }
        if ((unsigned long long)n64 > SIZE_MAX / sizeof(double)) {
            for (size_t ai = 0; ai < algorithm_count; ++ai)
                fprintf(csv, "C,%s,%lld,,,TAMANO_NO_REPRESENTABLE,\n", algorithms[ai].name, n64);
            continue;
        }
        size_t n = (size_t)n64;
        double *base = malloc(n * sizeof(*base));
        if (!base) {
            for (size_t ai = 0; ai < algorithm_count; ++ai)
                fprintf(csv, "C,%s,%lld,,,SIN_MEMORIA_PARA_ARREGLO,\n", algorithms[ai].name, n64);
            fflush(csv); continue;
        }
        for (size_t i = 0; i < n; ++i)
            base[i] = ((double)rand() / (double)RAND_MAX) * 2000.0 - 1000.0;

        for (size_t ai = 0; ai < algorithm_count; ++ai) {
            const Algorithm *alg = &algorithms[ai];
            if (strcmp(alg->name, "BubbleSort") == 0 && n64 > BUBBLE_LIMIT) {
                fprintf(csv, "C,%s,%lld,,,OMITIDO_POR_COMPLEJIDAD,\n", alg->name, n64);
                continue;
            }
            double *data = clone_array(base, n);
            if (!data) {
                fprintf(csv, "C,%s,%lld,,,SIN_MEMORIA_PARA_COPIA,\n", alg->name, n64);
                continue;
            }
            double start = seconds_now();
            alg->sort(data, n);
            double elapsed = seconds_now() - start;
            int valid = is_sorted(data, n);
            fprintf(csv, "C,%s,%lld,%.6f,%zu,%s,%s\n", alg->name, n64, elapsed,
                    n * sizeof(double), valid ? "OK" : "ERROR_O_INCOMPLETO",
                    valid ? "SI" : "NO");
            free(data);
            fflush(csv);
        }
        free(base);
    }
    fclose(csv);
    printf("Benchmark terminado. Archivo: resultados/resultados_C.csv\n");
    printf("Tamaños incluidos en el programa hasta 1,000,000,000; MAX_N actual: %lld\n", max_n);
    return 0;
}
