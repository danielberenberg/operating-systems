#include "pqueue.djberenb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pqueue_test_1() {
    PQueue pqueue;
    DataNode *data;
    int priority;

    printf("start of test_1()\n");

    // initialize a pq
    pqueue.head = NULL;
    pqueue.tail = NULL;

    // enqueue data with priority = 1
    data = (DataNode *) malloc(sizeof(DataNode));
    strcpy(data->name, "data one");
    priority = 1;
    printf("enqueue %s %d\n",data->name,priority);
    enqueue(&pqueue, priority, data);
    
    // enqueue data with priority = 4
    data = (DataNode *) malloc(sizeof(DataNode));
    strcpy(data->name, "data four-a");
    priority = 4;
    printf("enqueue %s %d\n",data->name,priority);
    enqueue(&pqueue, priority, data);

    // enqueue data with priority = 2
    data = (DataNode *) malloc(sizeof(DataNode));
    strcpy(data->name, "data two");
    priority = 2;
    printf("enqueue %s %d\n",data->name,priority);
    enqueue(&pqueue, priority, data);

    // enqueue data with priority = 8
    data = (DataNode *) malloc(sizeof(DataNode));
    strcpy(data->name, "data eight");
    priority = 8;
    printf("enqueue %s %d\n",data->name,priority);
    enqueue(&pqueue, priority, data);

    // enqueue data with priority = 4
    data = (DataNode *) malloc(sizeof(DataNode));
    strcpy(data->name, "data four-b");
    priority = 4;
    printf("enqueue %s %d\n",data->name,priority);
    enqueue(&pqueue, priority, data);

    printQueue(&pqueue);
    priority = getMinPriority(&pqueue);
    data = (DataNode *) dequeue(&pqueue);
    while (data != NULL){
        printf("dequeue: %s, priority = %d\n", data->name, priority);
        priority = getMinPriority(&pqueue);
        data = (DataNode *) dequeue(&pqueue);
    }

    data = (DataNode *) dequeue(&pqueue);
    if (data != NULL){
        printf("ERROR: expected empty queue\n");
    }
    printf("end of test_1()\n\n");
}

void pqueue_test_2() {
    PQueue pqueue;
    DataNode *data;
    int priority;

    printf("start of test_2()\n");

    // initialize a pq
    pqueue.head = NULL;
    pqueue.tail = NULL;

    // enqueue data with priority = 4
    data = (DataNode *) malloc(sizeof(DataNode));
    strcpy(data->name, "data four-a");
    priority = 4;
    printf("enqueue %s %d\n",data->name,priority);
    enqueue(&pqueue, priority, data);
    
    // enqueue data with priority = 3
    data = (DataNode *) malloc(sizeof(DataNode));
    strcpy(data->name, "data three");
    priority = 3;
    printf("enqueue %s %d\n",data->name,priority);
    enqueue(&pqueue, priority, data);

    // enqueue data with priority = 10
    data = (DataNode *) malloc(sizeof(DataNode));
    strcpy(data->name, "data ten");
    priority = 10;
    printf("enqueue %s %d\n",data->name,priority);
    enqueue(&pqueue, priority, data);

    priority = getMinPriority(&pqueue);
    data = (DataNode *) dequeue(&pqueue);
    if (data == NULL){
        printf("ERROR: expect data to be non-null\n");
    } else {
        printf("dequeue: %s, priority = %d\n", data->name, priority);
    }
    // enqueue data with priority = 1
    data = (DataNode *) malloc(sizeof(DataNode));
    strcpy(data->name, "data one");
    priority = 1;
    printf("enqueue %s %d\n",data->name,priority);
    enqueue(&pqueue, priority, data);
    
    priority = getMinPriority(&pqueue);
    if (data == NULL){
        printf("ERROR: expect data to be non-null\n");
    } else {
        printf("dequeue: %s, priority = %d\n", data->name, priority);
    }

    // enqueue data with priority = 9
    data = (DataNode *) malloc(sizeof(DataNode));
    strcpy(data->name, "data six");
    priority = 9;
    printf("enqueue %s %d\n",data->name,priority);
    enqueue(&pqueue, priority, data);

    priority = getMinPriority(&pqueue);
    data = (DataNode *) dequeue(&pqueue);
    if (data == NULL){
        printf("ERROR: expect data to be non-null\n");
    } else {
        printf("dequeue: %s, priority = %d\n", data->name, priority);
    }

    priority = getMinPriority(&pqueue);
    printQueue(&pqueue);
    while (data != NULL){
        printf("dequeue: %s, priority = %d\n", data->name, priority);
        priority = getMinPriority(&pqueue);
        data = (DataNode *) dequeue(&pqueue);
    }
    printf("end of test_2()\n\n");
}

void pqueue_test_3() {
    PQueue pqueue;
    DataNode *data;
    char buf[16];
    int i, ival, numvals;
    int vals[16];
    int priority;

    printf("start of pqueue_test_3()\n");
    vals[0] = 5; 
    vals[1] = 3;
    vals[2] = 17;
    vals[3] = 35;
    vals[4] = 10;
    vals[5] = 8;
    vals[6] = 20;
    vals[7] = 41;
    vals[8] = 9;
    vals[9] = 2;

    pqueue.head = NULL;
    pqueue.tail = NULL;

    numvals = 10;
    for (i=0; i<numvals; ++i){
        ival = vals[i];
        sprintf(buf, "%dA", ival);
        data = (DataNode *) malloc(sizeof(DataNode));
        strcpy(data->name, buf);
        enqueue(&pqueue, ival, data);

        if (i%2 == 0) {
            priority = getMinPriority(&pqueue);
            data = (DataNode *) dequeue(&pqueue);
            if (data != NULL){
                printf("dequeued %d %s\n",priority, data->name);
            }
        }
    }
    printQueue(&pqueue);
    for (i=0; i<6; ++i){
        data = (DataNode *) dequeue(&pqueue);
    }
    printQueue(&pqueue);
}

void pqueue_test_4() {
    PQueue pqueue;
    DataNode *data;
    char buf[16];
    int i, ival, numvals;
    int vals[16];
    int priority;

    printf("start of pqueue_test_4()\n");
    vals[0] = 5; 
    vals[1] = 3;
    vals[2] = 17;
    vals[3] = 35;
    vals[4] = 10;
    
    numvals = 5;
    pqueue.head = NULL;
    pqueue.tail = NULL;

    for (i=0; i<numvals; ++i){
        ival = vals[i];
        sprintf(buf, "%dA", ival);
        data = (DataNode *) malloc(sizeof(DataNode));
        strcpy(data->name, buf);
        enqueue(&pqueue, ival, data);

        sprintf(buf, "%dB", ival);
        data = (DataNode *) malloc(sizeof(DataNode));
        strcpy(data->name, buf);
        enqueue(&pqueue, ival, data);
    }

    printQueue(&pqueue);
}

int main() {
    pqueue_test_1();
    pqueue_test_2();
    pqueue_test_3();
    pqueue_test_4();
}
