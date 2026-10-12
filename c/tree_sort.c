#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <errno.h>

/* Tree Sort: inserta valores en un árbol binario y los recupera en inorden. */
typedef struct Nodo {
    double valor;
    struct Nodo *izq, *der;
} Nodo;

static void tree_sort(double *a, size_t n) {
    if (n < 2) return;
    Nodo *raiz = NULL;
    Nodo **nodos = calloc(n, sizeof(*nodos)); /* Guarda nodos para liberarlos al final. */
    Nodo **pila = malloc(n * sizeof(*pila));  /* Pila del recorrido inorden. */
    if (!nodos || !pila) {
        fprintf(stderr, "Memoria insuficiente para Tree Sort.\\n");
        free(nodos); free(pila); return;
    }
    size_t usados = 0;
    for (size_t i = 0; i < n; ++i) {
        Nodo *nuevo = malloc(sizeof(*nuevo));
        if (!nuevo) {
            fprintf(stderr, "No se pudo reservar un nodo.\\n");
            for (size_t j = 0; j < usados; ++j) free(nodos[j]);
            free(nodos); free(pila); return;
        }
        nuevo->valor = a[i]; nuevo->izq = nuevo->der = NULL;
        nodos[usados++] = nuevo;
        if (!raiz) raiz = nuevo;
        else {
            Nodo *actual = raiz;
            for (;;) {
                if (nuevo->valor < actual->valor) {
                    if (!actual->izq) { actual->izq = nuevo; break; }
                    actual = actual->izq;
                } else {
                    if (!actual->der) { actual->der = nuevo; break; }
                    actual = actual->der;
                }
            }
        }
    }

    size_t tope = 0, salida = 0;
    Nodo *actual = raiz;
    while (actual || tope) {
        while (actual) { pila[tope++] = actual; actual = actual->izq; }
        actual = pila[--tope];
        a[salida++] = actual->valor;
        actual = actual->der;
    }
    for (size_t j = 0; j < usados; ++j) free(nodos[j]);
    free(nodos); free(pila);
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
    FILE *csv = fopen("resultados/RESULTADO_TreeSort_C.csv", "w");
    if (!csv) { perror("No se pudo abrir el CSV"); return 1; }
    fprintf(csv, "algoritmo,n,tiempo_s,memoria_arreglo_bytes,estado,ordenado\\n");
    srand(20261011);
    for (size_t s = 0; s < sizeof(SIZES)/sizeof(SIZES[0]); ++s) {
        long long n64 = SIZES[s];
        if (n64 > max_n) {
            fprintf(csv, "TreeSort,n=%lld,,,OMITIDO_POR_MAX_N,\\n", n64);
            continue;
        }
        if (strcmp("TreeSort", "Bubble Sort") == 0 && n64 > 10000) {
            fprintf(csv, "TreeSort,%lld,,,OMITIDO_POR_COMPLEJIDAD,\\n", n64); continue;
        }
        size_t n = (size_t)n64;
        double *a = malloc(n * sizeof(*a));
        if (!a) { fprintf(csv, "TreeSort,%lld,,,SIN_MEMORIA,\\n", n64); continue; }
        for (size_t i = 0; i < n; ++i) a[i] = ((double)rand()/RAND_MAX)*2000.0-1000.0;
        double inicio = reloj_segundos();
        tree_sort(a, n);
        double duracion = reloj_segundos() - inicio;
        int ok = esta_ordenado(a, n);
        fprintf(csv, "TreeSort,%lld,%.6f,%zu,%s,%s\\n", n64, duracion,
                n*sizeof(*a), ok ? "OK" : "ERROR_O_INCOMPLETO", ok ? "SI" : "NO");
        free(a);
    }
    fclose(csv);
    printf("Resultados guardados en resultados/RESULTADO_TreeSort_C.csv\\n");
    return 0;
}
