
typedef void (*test_funct)(void *);

double fcyc(test_funct f, void* argp);


void set_fcyc_clear_cache(int clear);


void set_fcyc_cache_size(int bytes);


void set_fcyc_cache_block(int bytes);


void set_fcyc_compensate(int compensate_arg);


void set_fcyc_k(int k);


void set_fcyc_maxsamples(int maxsamples_arg);


void set_fcyc_epsilon(double epsilon_arg);




