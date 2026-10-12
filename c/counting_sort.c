#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <errno.h>

/* Counting Sort para reales cuantizados a centésimas: 7.25 se vuelve la clave 725. */
static void counting_sort(double *a, size_t n) {
    if (!n) return;
    long long *claves = malloc(n * sizeof(*claves));
    if (!claves) { fprintf(stderr, "Memoria insuficiente para claves.\\n"); return; }
    long long minimo = LLONG_MAX, maximo = LLONG_MIN;
    for (size_t i = 0; i < n; ++i) {
        claves[i] = llround(a[i] * 100.0);
        if (claves[i] < minimo) minimo = claves[i];
        if (claves[i] > maximo) maximo = claves[i];
    }
    unsigned long long rango64 = (unsigned long long)(maximo - minimo) + 1ULL;
    if (rango64 > 5000000ULL) {
        fprintf(stderr, "Counting Sort omitido: rango de claves demasiado grande.\\n");
        free(claves); return;
    }
    size_t rango = (size_t)rango64;
    size_t *cuentas = calloc(rango, sizeof(*cuentas));
    if (!cuentas) { fprintf(stderr, "Memoria insuficiente para conteos.\\n"); free(claves); return; }
    for (size_t i = 0; i < n; ++i) cuentas[(size_t)(claves[i] - minimo)]++;
    size_t pos = 0;
    for (size_t i = 0; i < rango; ++i)
        for (size_t rep = 0; rep < cuentas[i]; ++rep)
            a[pos++] = (double)((long long)i + minimo) / 100.0;
    free(cuentas); free(claves);
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
    FILE *csv = fopen("resultados/RESULTADO_CountingSort_C.csv", "w");
    if (!csv) { perror("No se pudo abrir el CSV"); return 1; }
    fprintf(csv, "algoritmo,n,tiempo_s,memoria_arreglo_bytes,estado,ordenado\\n");
    srand(20261011);
    for (size_t s = 0; s < sizeof(SIZES)/sizeof(SIZES[0]); ++s) {
        long long n64 = SIZES[s];
        if (n64 > max_n) {
            fprintf(csv, "CountingSort,n=%lld,,,OMITIDO_POR_MAX_N,\\n", n64);
            continue;
        }
        if (strcmp("CountingSort", "Bubble Sort") == 0 && n64 > 10000) {
            fprintf(csv, "CountingSort,%lld,,,OMITIDO_POR_COMPLEJIDAD,\\n", n64); continue;
        }
        size_t n = (size_t)n64;
        double *a = malloc(n * sizeof(*a));
        if (!a) { fprintf(csv, "CountingSort,%lld,,,SIN_MEMORIA,\\n", n64); continue; }
        for (size_t i = 0; i < n; ++i) a[i] = ((double)rand()/RAND_MAX)*2000.0-1000.0;
        double inicio = reloj_segundos();
        counting_sort(a, n);
        double duracion = reloj_segundos() - inicio;
        int ok = esta_ordenado(a, n);
        fprintf(csv, "CountingSort,%lld,%.6f,%zu,%s,%s\\n", n64, duracion,
                n*sizeof(*a), ok ? "OK" : "ERROR_O_INCOMPLETO", ok ? "SI" : "NO");
        free(a);
    }
    fclose(csv);
    printf("Resultados guardados en resultados/RESULTADO_CountingSort_C.csv\\n");
    return 0;
}
