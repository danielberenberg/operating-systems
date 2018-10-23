#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define NUMVALS    1024
#define RAND_LOW   1     // random low  bound
#define RAND_HI    5000  // random high bound

typedef struct {  // packages data for thread startup
    int startIndex;
    int endIndex;
    int thread_id;   // give a name to each thread for display purposes
} SortInfo;

int sortme[NUMVALS];  // global array to sort
int finished[NUMVALS];

int genRand(int low, int high){
    /* generate a random integer r in [low, high] 
     *
     * args:
     *     :int low - lowest value of interval
     *     :int high - highest value of interval
     * returns:
     *     :(int) - random number
     */
    double r1, r2;
    r1 = drand48();
    r2 = (1 + high - low) * r1;
    return low + floor(r2);
}

void *sorter(void *params){
    /* perform a simple insertion sort on the global sortme array
     args:
         :void *params - the data that will be coerced into SortInfo
     returns:
         :Nothing, sorting is done in place
    */
    SortInfo *si;
    si = (SortInfo *) params;
    printf("[sorter] sorting from %d to %d (id=%d)\n", si->startIndex, si->endIndex, si->thread_id);
    int i, j, val;
    for (i=si->startIndex; i < si->endIndex + 1; i++){
       val = sortme[i];
       j = i - 1;
       while (j >= si->startIndex && sortme[j] > val){
           sortme[j+1] = sortme[j];
           j--;
       }
       sortme[j+1] = val;
    }
    pthread_exit(0);
}

void *merger(void *params){
    /*
     * merge two sorted halves of an array into a fully sorted array
     * args:
     *     :void *params - the data that will be coerced into an integer (the second start index)
     * retunrs:
     *     :Nothing, sorting is done in place
     */

    int i, j, k, border;
    j = border =*((int *) params);
    i = k = 0;
    printf("[merger] merging the sorted subarrays\n");
    while (i < border && j < NUMVALS){
        if (sortme[i] < sortme[j]){ // place a member of second half in first half
            // swap the values
            finished[k] = sortme[i];
            i++;
        }
        else { 
            finished[k] = sortme[j];
            j++; 
        }
        k++;
    }

    while (i < border){
       finished[k] = sortme[i];
       i++;
       k++;
    }

    while(j < NUMVALS){
        finished[k] = sortme[j];
        j++;
        k++;
    }
    pthread_exit(0);
}

int checkSorted(){
    int i;
    for (i=1; i<NUMVALS; ++i){
        if (finished[i] < finished[i-1]){
            return 0;
        }
    }
    return 1;
}

int main(){
    long seed;
    time(&seed);
    srand48(seed);
    int i;
    for (i = 0; i < NUMVALS; ++i){
        sortme[i] = genRand(RAND_LOW, RAND_HI);
    }
    printf("[*] initialized array\n");

    SortInfo si1, si2;
    si1.startIndex = 0;
    si1.endIndex = NUMVALS/2;
    si1.thread_id = 1;
    si2.startIndex = si1.endIndex + 1; 
    si2.endIndex = NUMVALS - 1;
    si2.thread_id = 2;
    
    pthread_t tid1, tid2, mergeid;      /* thread ids */
    pthread_attr_t attr;       /* thread attributes */

    /* default attributes init */
    pthread_attr_init(&attr);

    /* create/dispatch threads */
    pthread_create(&tid1, &attr, sorter, &si1);  // thread 1
    pthread_create(&tid2, &attr, sorter, &si2);  // thread 2
    
    // wait for termination of each sorter    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    int secondHalf = si2.startIndex;
    pthread_create(&mergeid, &attr, merger, &secondHalf);
    pthread_join(mergeid, NULL);
    
    printf("array is sorted: %d\n", checkSorted());


    return 0;
}


