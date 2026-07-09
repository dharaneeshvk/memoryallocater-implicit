#ifndef __CONFIG_H_
#define __CONFIG_H_


#define DEFAULT_TRACEFILES \
  "amptjp-bal.rep",\
  "cccp-bal.rep",\
  "cp-decl-bal.rep",\
  "expr-bal.rep",\
  "coalescing-bal.rep",\
  "random-bal.rep",\
  "random2-bal.rep",\
  "binary-bal.rep",\
  "binary2-bal.rep",\
  "realloc-bal.rep",\
  "realloc2-bal.rep"
#define TRACEDIR "./traces/"


#define AVG_LIBC_THRUPUT      600E3  

 
#define UTIL_WEIGHT .60


#define ALIGNMENT 8  


#define MAX_HEAP (20*(1<<20))  /* 20 MB */


#define USE_FCYC   0   
#define USE_ITIMER 0   
#define USE_GETTOD 1   

#endif 
