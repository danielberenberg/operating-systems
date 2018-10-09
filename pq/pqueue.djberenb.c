#include "pqueue.djberenb.h"
#include <stdio.h>
#include <stdlib.h>

void createNode(PQueueNode **node, int priority, void *data, PQueueNode *next){
    /*
     * Allocate memory for and populate the fields of a new
     * Priority Queue node. Intentionally left out of .h as a private API
     *
     * args:
     *     :PQueueNode **node - memory location for this new node
     *     :int priority      - priority of the node
     *     :void *data        - the data that we want to store
     *     :PQueueNode *next  - pointer to the next node in the priority queue
     *
     * returns:
     *     :(void)
     */

     PQueueNode *n;
     n = (PQueueNode *) malloc(sizeof(PQueueNode));
     n->data = data;
     n->next = next;
     n->priority = priority;
     *node = n; 
}

int enqueue(PQueue *pq, int priority, void *data){
/* put node in queue in priority order;
 *  - in case of tie in priority, input node is placed at
 *    the end of the sequence of nodes with the spec. priority 
 * 
 * args:
 *     :PQueue *pq - priority queue to manipulate
 *     :int priority - priority of the data
 *     :void *data - the data to store
 * returns:
 *     : 0 always
 */
    PQueueNode *node;
    // empty queue!
    if (pq->head == NULL){
       createNode(&node, priority, data, NULL); 
       pq->head = node;
       pq->tail = node;
       return 0;
    }
    // input has lowest priority in all of queue
    if (pq->tail->priority <= priority){
        createNode(&node, priority, data, NULL);
        pq->tail->next = node;
        pq->tail = node;
        return 0;
    }
    // input will appear somehwere in the middle of the queue
    PQueueNode *curr, *prev;
    curr = pq->head;
    prev = NULL;
    // find the location to place the node
    while (priority >= curr->priority){
        prev = curr;
        curr = curr->next;
    } // loop finishes fixed on the node that will directly follow input
    createNode(&node, priority, data, curr);
    if (prev != NULL){
        prev->next = node;
    }
    return 0;

}

void *dequeue(PQueue *pq){
/* return NULL if queue is empty; otherwise return the data (the DataNode)
 * from the first node in the queue (since the queue is maintained in priority 
 * order (from lowest to highest) this will be the data from the node having
 * the smallest priority value
 *
 * args:
 *     :PQueue *pq - the queue to manipulate
 * returns:
 *     :void * -- the data, NULL if queue is empty
 */
    // the queue is empty
    if (pq->head == NULL){
        return NULL;
    }
    void *data = pq->head->data;
    PQueueNode *nxt, *head;
    head = pq->head;
    nxt = pq->head->next;
    
    // have a priority queue of length 1, will become 0
    // ==> need to update the tail to NULL
    if (pq->tail == pq->head){
        pq->tail = NULL; 
    }
    // erase the head
    free(head);
    // next element becomes the head 
    pq->head = nxt;

    return data;
    
}

int printQueue(PQueue *pq){
/*   print queue from head to tail
 *
 *   args: 
 *       :PQueue *pq - the priority queue to print
 *   returns:
 *          :0 always 
 */ 
    PQueueNode *curr;
    DataNode *data;
    curr = pq->head;
    if (curr == NULL){
        printf("( )\n");
    }
    printf("( ");
    while (curr != NULL){
        data = (DataNode *) curr->data;
        printf("[%d|%s]",curr->priority, data->name);
        if (curr->next != NULL){
            printf("-->");
        }
        curr = curr->next;
    }
    printf(" )\n");
    return 0;
}

void *peek(PQueue *pq){
/*  return the first node in the queue, but do not remove it; 
 *   args: 
 *       :PQueue *pq - the priority queue to print
 *  returns:
 *     :data of first node in the queue, NULL if empty
 */
    if (pq->head == NULL){
        return NULL;
    }
    return pq->head->data;
}

int getMinPriority(PQueue *pq){
/* return priority of first node in queue; return -1 if queue is empty
 *   args: 
 *       :PQueue *pq - the priority queue to manipulate
 *   returns:
 *       :priority of first node, -1 if empty
 */
    if (pq->head == NULL){
        return -1;
    }
    return pq->head->priority;
}
