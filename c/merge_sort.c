#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <errno.h>

/* Merge Sort: divide el arreglo en mitades y combina segmentos ordenados. */
static void merge_range(double *a, double *tmp, size_t inicio, size_t fin) {
    if (fin - inicio < 2) return; /* Caso base: cero o un elemento. */
    size_t medio = inicio + (fin - inicio) / 2;
    merge_range(a, tmp, inicio, medio);
    merge_range(a, tmp, medio, fin);

    size_t i = inicio, j = medio, k = inicio;
    while (i < medio && j < fin)
        tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i < medio) tmp[k++] = a[i++];
    while (j < fin) tmp[k++] = a[j++];
    memcpy(a + inicio, tmp + inicio, (fin - inicio) * sizeof(*a));
}
static void merge_sort(double *a, size_t n) {
    double *temporal = malloc(n * sizeof(*temporal));
    if (!temporal) { fprintf(stderr, "Memoria insuficiente para Merge Sort.\\n"); return; }
    merge_range(a, temporal, 0, n);
    free(temporal); /* Libera el arreglo auxiliar. */
}
/* Tamaños requeridos por la práctica. MAX_N limita la memoria reservada al ejecutar. */
static const long long SIZES[] = {100000LL, 1000000LL, 10000000LL, 100000000LL, 1000000000LL};
static double reloj_segundos(void) {
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}
static int esta_ordenado(const double *a, size_t n) {
    for (size_t i = 1; i < n; ++i) if (a[i - 1] > a[i]) return 0;
    return 1;
}
int main(void) {
    long long max_n = 100000LL;
    const char *limite = getenv("MAX_N");
    if (limite && *limite) max_n = atoll(limite);
    system("mkdir -p resultados");
    FILE *csv = fopen("resultados/RESULTADO_MergeSort_C.csv", "w");
    if (!csv) { perror("No se pudo abrir el CSV"); return 1; }
    fprintf(csv, "algoritmo,n,tiempo_s,memoria_arreglo_bytes,estado,ordenado\\n");
    srand(20261011);
    for (size_t s = 0; s < sizeof(SIZES)/sizeof(SIZES[0]); ++s) {
        long long n64 = SIZES[s];
        if (n64 > max_n) {
            fprintf(csv, "MergeSort,n=%lld,,,OMITIDO_POR_MAX_N,\\n", n64);
            continue;
        }
        if (strcmp("MergeSort", "Bubble Sort") == 0 && n64 > 10000) {
            fprintf(csv, "MergeSort,%lld,,,OMITIDO_POR_COMPLEJIDAD,\\n", n64); continue;
        }
        size_t n = (size_t)n64;
        double *a = malloc(n * sizeof(*a));
        if (!a) { fprintf(csv, "MergeSort,%lld,,,SIN_MEMORIA,\\n", n64); continue; }
        for (size_t i = 0; i < n; ++i) a[i] = ((double)rand()/RAND_MAX)*2000.0-1000.0;
        double inicio = reloj_segundos();
        merge_sort(a, n);
        double duracion = reloj_segundos() - inicio;
        int ok = esta_ordenado(a, n);
        fprintf(csv, "MergeSort,%lld,%.6f,%zu,%s,%s\\n", n64, duracion,
                n*sizeof(*a), ok ? "OK" : "ERROR_O_INCOMPLETO", ok ? "SI" : "NO");
        free(a);
    }
    fclose(csv);
    printf("Resultados guardados en resultados/RESULTADO_MergeSort_C.csv\\n");
    return 0;
}
