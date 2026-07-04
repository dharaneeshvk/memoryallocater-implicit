

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include "clock.h"



#if defined(__i386__)  

static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;


void access_counter(unsigned *hi, unsigned *lo)
{
    asm("rdtsc; movl %%edx,%0; movl %%eax,%1"   
	: "=r" (*hi), "=r" (*lo)                
	: /* No input */                        
	: "%edx", "%eax");
}

void start_counter()
{
    access_counter(&cyc_hi, &cyc_lo);
}

double get_counter()
{
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    access_counter(&ncyc_hi, &ncyc_lo);

    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double) hi * (1 << 30) * 4 + lo;
    if (result < 0) {
	fprintf(stderr, "Error: counter returns neg value: %.0f\n", result);
    }
    return result;
}

#elif defined(__alpha)


static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;



static unsigned int counterRoutine[] =
{
    0x601fc000u,
    0x401f0000u,
    0x6bfa8001u
};

static unsigned int (*counter)(void)= (void *)counterRoutine;


void start_counter()
{
    cyc_hi = 0;
    cyc_lo = counter();
}

double get_counter()
{
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;
    ncyc_lo = counter();
    ncyc_hi = 0;
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double) hi * (1 << 30) * 4 + lo;
    if (result < 0) {
	fprintf(stderr, "Error: Cycle counter returning negative value: %.0f\n", result);
    }
    return result;
}

#else


void start_counter()
{
    printf("ERROR: You are trying to use a start_counter routine in clock.c\n");
    printf("that has not been implemented yet on this platform.\n");
    printf("Please choose another timing package in config.h.\n");
    exit(1);
}

double get_counter() 
{
    printf("ERROR: You are trying to use a get_counter routine in clock.c\n");
    printf("that has not been implemented yet on this platform.\n");
    printf("Please choose another timing package in config.h.\n");
    exit(1);
}
#endif





double ovhd()
{
    int i;
    double result;

    for (i = 0; i < 2; i++) {
	start_counter();
	result = get_counter();
    }
    return result;
}


double mhz_full(int verbose, int sleeptime)
{
    double rate;

    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6*sleeptime);
    if (verbose) 
	printf("Processor clock rate ~= %.1f MHz\n", rate);
    return rate;
}

double mhz(int verbose)
{
    return mhz_full(verbose, 2);
}



static double cyc_per_tick = 0.0;

#define NEVENT 100
#define THRESHOLD 1000
#define RECORDTHRESH 3000

static void callibrate(int verbose)
{
    double oldt;
    struct tms t;

    clock_t oldc;
    

    times(&t);
    oldc = t.tms_utime;
    start_counter();
    oldt = get_counter();
    int e=0;
    while (e <NEVENT) {
	double newt = get_counter();

	if (newt-oldt >= THRESHOLD) {

	    clock_t newc;

	    times(&t);
	    newc = t.tms_utime;
	    if (newc > oldc) 
        {
		double cpt = (newt-oldt)/(newc-oldc);
		if ((cyc_per_tick == 0.0 || cyc_per_tick > cpt) && cpt > RECORDTHRESH)
		    cyc_per_tick = cpt;
		
		e++;
		oldc = newc;
	    }
	    oldt = newt;
	}
    }
    if (verbose)
	printf("Setting cyc_per_tick to %f\n", cyc_per_tick);
}

static clock_t start_tick = 0;

void start_comp_counter() 
{
    struct tms t;

    if (cyc_per_tick == 0.0)
	callibrate(0);
    times(&t);
    start_tick = t.tms_utime;
    start_counter();
}

double get_comp_counter() 
{
    double time = get_counter();
    double ctime;
    struct tms t;

    clock_t ticks;

    times(&t);

    ticks = t.tms_utime - start_tick;
    ctime = time - ticks*cyc_per_tick;
    
    return ctime;
}

