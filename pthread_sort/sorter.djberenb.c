#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define NUMVALS 1024
#define NUMTHREADS 2     // number of threads performing the sorting 
#define RAND_LOW   1
#define RAND_HI    5000

typedef struct {  // packages data for thread startup
    int start_index;
    int end_index;
} SortInfo;

int genRand(int low, int high){
    // generate random integer r in [low, high] 
    double r1, r2;
    r1 = drand48();
    r2 = (1 + high - low) * r1;
    return low + floor(r2);
}

int main(int argc, char *argv[]){
    //long seed;
    //time(&seed);
    //srand48(seed);
    
    printf("%d\n", genRand(0,5));
}


