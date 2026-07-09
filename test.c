#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mm.h"
#include "memlib.h"

int main() {
    mem_init();
    mm_init();

    void *p1 = mm_malloc(32);
    printf("p1 = %p\n", p1);

    void *p2 = mm_malloc(64);
    printf("p2 = %p\n", p2);

    mm_free(p1);

    void *p3 = mm_malloc(16);
    printf("p3 = %p\n", p3);  

    mm_free(p2);
    mm_free(p3);

    void *p4 = mm_malloc(32);
    void *p5 = mm_malloc(32);
    mm_free(p4);
    mm_free(p5);
    void *p6 = mm_malloc(60);
    printf("p6 = %p, p4 = %p \n", p6, p4);

    mem_init();
    mm_init();

    clock_t start = clock();

    void *ptrs[1000];
    for (int i = 0; i < 1000; i++)
        ptrs[i] = mm_malloc(rand() % 512 + 1);
    for (int i = 0; i < 1000; i+=2)
        mm_free(ptrs[i]);
    for (int i = 0; i < 500; i++)
        ptrs[i] = mm_malloc(rand() % 200 + 1);

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("10000 operations in %.4f seconds\n", time_taken);
    printf("Throughput: %.0f ops/sec\n", 10000.0 / time_taken);

    return 0;
}