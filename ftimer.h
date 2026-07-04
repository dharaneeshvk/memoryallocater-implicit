
typedef void (*ftimer_test_funct)(void *); 


double ftimer_itimer(ftimer_test_funct f, void *argp, int n);



double ftimer_gettod(ftimer_test_funct f, void *argp, int n);

