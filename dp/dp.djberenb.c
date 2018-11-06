#include "stdio.h"
#include "pthread.h"
#include "stdlib.h"
#include "unistd.h"

#define NUM_PHILOSOPHERS  5
#define RUNTIME          10 
#define EAT_TIME          2
#define THINK_TIME        1
#define True              1

pthread_mutex_t forks[NUM_PHILOSOPHERS];

void *philosopher(void *param){
    /*
     * poor solution to the dining philosophers problem, causes deadlock
     */
    int *myID, leftIdx, rightIdx;
    myID = (int *) param;
    leftIdx = *myID;
    rightIdx = (*myID + 1) % NUM_PHILOSOPHERS;
    printf("(bad) [P%d] start, forks are %d and %d\n", *myID,leftIdx, rightIdx);

    while ( 1 ){
        pthread_mutex_lock(&forks[leftIdx]);
        usleep(2000);
        pthread_mutex_lock(&(forks[rightIdx]));
        printf("(bad) [P%d] eating for %d secs\n",*myID, EAT_TIME);
        sleep(EAT_TIME);
        pthread_mutex_unlock(&(forks[leftIdx]));
        pthread_mutex_unlock(&(forks[rightIdx]));
        printf("(bad) [P%d] thinking for %d secs\n", *myID, THINK_TIME); 
        sleep(THINK_TIME);
    }
}

void *philosopher_good(void *param){
    /*
     * trylock solution to philosopher problem
     */
    int *myID, left, right;

    myID = (int *) param;
    left = *myID;
    right = (*myID + 1) % NUM_PHILOSOPHERS;
    printf("(good) [P%d] start, forks are %d and %d\n", *myID,left, right);
     
    int left_clear, right_clear;
    while ( 1 ){
        left_clear = pthread_mutex_trylock(&forks[left]);
        right_clear = pthread_mutex_trylock(&forks[right]);
        if ((left_clear==0) && (right_clear==0)){
            printf("(good) [P%d] eating for %d secs\n",*myID, EAT_TIME);
            sleep(EAT_TIME);
            pthread_mutex_unlock(&forks[left]);
            pthread_mutex_unlock(&forks[right]);
            printf("(good) [P%d] thinking for %d secs\n", *myID, THINK_TIME); 
            sleep(THINK_TIME);
        }
        else {pthread_mutex_unlock(&forks[left]); pthread_mutex_unlock(&forks[right]);}
    }

}



int main(){
    int rtnval, i;
    int id[NUM_PHILOSOPHERS];
    int id2[NUM_PHILOSOPHERS];
    pthread_t tid[NUM_PHILOSOPHERS];
    pthread_t tid2[NUM_PHILOSOPHERS];
    pthread_attr_t attr;       /* thread attributes */
    // instantiate mutex forks
    for (i=0; i<NUM_PHILOSOPHERS; ++i){
        rtnval = pthread_mutex_init(&(forks[i]), NULL);
        if (rtnval != 0){
            printf("error initializing mutex %d\n",i);
            return(8);
        }
    }
    // run the bad philosophes algorithm
    pthread_attr_init(&attr); /* default attributes init */

    //printf("---- bad philosophers ----\n"); 
    //for (i=0; i<NUM_PHILOSOPHERS; ++i){
    //    id[i] = i;
    //    pthread_create(&(tid[i]), &attr, philosopher, &(id[i]));
    //}
    //sleep(5);
    //for (i=0; i<NUM_PHILOSOPHERS; ++i){
    //    pthread_kill(tid[i], 0);
    //    printf("[P%d] killed\n",i);
    //}

    printf("---- good philosophers ----\n");
    for (i=0; i<NUM_PHILOSOPHERS; ++i){
        id2[i] = i;
        pthread_create(&(tid2[i]), &attr, philosopher_good, &(id2[i]));
    }
    for (i=0; i<NUM_PHILOSOPHERS; ++i){
        pthread_join(tid2[i],NULL);
    } 
}
