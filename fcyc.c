
#include <stdlib.h>
#include <sys/times.h>
#include <stdio.h>

#include "fcyc.h"
#include "clock.h"

#define K 3                  
#define MAXSAMPLES 20        
#define EPSILON 0.01         
#define COMPENSATE 0         
#define CLEAR_CACHE 0        
#define CACHE_BYTES (1<<19)  
#define CACHE_BLOCK 32       

static int kbest = K;
static int maxsamples = MAXSAMPLES;
static double epsilon = EPSILON;
static int compensate = COMPENSATE;
static int clear_cache = CLEAR_CACHE;
static int cache_bytes = CACHE_BYTES;
static int cache_block = CACHE_BLOCK;

static int *cache_buf = NULL;

static double *values = NULL;
static int samplecount = 0;

#define KEEP_VALS 0
#define KEEP_SAMPLES 0

#if KEEP_SAMPLES
static double *samples = NULL;
#endif


static void init_sampler()
{
    if (values)
	free(values);
    values = calloc(kbest, sizeof(double));
#if KEEP_SAMPLES
    if (samples)
	free(samples);
    
    samples = calloc(maxsamples+kbest, sizeof(double));
#endif
    samplecount = 0;
}


static void add_sample(double val)
{
    int pos = 0;
    if (samplecount < kbest) {
	pos = samplecount;
	values[pos] = val;
    } else if (val < values[kbest-1]) {
	pos = kbest-1;
	values[pos] = val;
    }
#if KEEP_SAMPLES
    samples[samplecount] = val;
#endif
    samplecount++;
    while (pos > 0 && values[pos-1] > values[pos]) {
	double temp = values[pos-1];
	values[pos-1] = values[pos];
	values[pos] = temp;
	pos--;
    }
}

static int has_converged()
{
    return
	(samplecount >= kbest) &&
	((1 + epsilon)*values[0] >= values[kbest-1]);
}


static volatile int sink = 0;

static void clear()
{
    int x = sink;
    int *cptr, *cend;
    int incr = cache_block/sizeof(int);
    if (!cache_buf) {
	cache_buf = malloc(cache_bytes);
	if (!cache_buf) {
	    fprintf(stderr, "Fatal error.  Malloc returned null when trying to clear cache\n");
	    exit(1);
	}
    }
    cptr = (int *) cache_buf;
    cend = cptr + cache_bytes/sizeof(int);
    while (cptr < cend) {
	x += *cptr;
	cptr += incr;
    }
    sink = x;
}


double fcyc(test_funct f, void *argp)
{
    double result;
    init_sampler();
    if (compensate) {
	do {
	    double cyc;
	    if (clear_cache)
		clear();
	    start_comp_counter();
	    f(argp);
	    cyc = get_comp_counter();
	    add_sample(cyc);
	} while (!has_converged() && samplecount < maxsamples);
    } else {
	do {
	    double cyc;
	    if (clear_cache)
		clear();
	    start_counter();
	    f(argp);
	    cyc = get_counter();
	    add_sample(cyc);
	} while (!has_converged() && samplecount < maxsamples);
    }
#ifdef DEBUG
    {
	int i;
	printf(" %d smallest values: [", kbest);
	for (i = 0; i < kbest; i++)
	    printf("%.0f%s", values[i], i==kbest-1 ? "]\n" : ", ");
    }
#endif
    result = values[0];
#if !KEEP_VALS
    free(values); 
    values = NULL;
#endif
    return result;  
}



void set_fcyc_clear_cache(int clear)
{
    clear_cache = clear;
}


void set_fcyc_cache_size(int bytes)
{
    if (bytes != cache_bytes) {
	cache_bytes = bytes;
	if (cache_buf) {
	    free(cache_buf);
	    cache_buf = NULL;
	}
    }
}


void set_fcyc_cache_block(int bytes) {
    cache_block = bytes;
}



void set_fcyc_compensate(int compensate_arg)
{
    compensate = compensate_arg;
}


void set_fcyc_k(int k)
{
    kbest = k;
}


void set_fcyc_maxsamples(int maxsamples_arg)
{
    maxsamples = maxsamples_arg;
}


void set_fcyc_epsilon(double epsilon_arg)
{
    epsilon = epsilon_arg;
}





