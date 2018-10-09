#include "pqueue.djberenb.h"
#include <stdio.h>
#include <stdlib.h>

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
    printf("[enqueue] not implemented!\n");
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
    printf("[dequeue] not implemented!\n");
    return NULL;
}

int printQueue(PQueue *pq){
/*   print queue from head to tail
 *
 *   args: 
 *       :PQueue *pq - the priority queue to print
 *   returns:
 *          :0 always 
 */ 
    printf("[printQueue] not implemented!\n");
    return -999;
}

void *peek(PQueue *pq){
/*  return the first node in the queue, but do not remove it; 
 *   args: 
 *       :PQueue *pq - the priority queue to print
 *  returns:
 *     :data of first node in the queue, NULL if empty
 */
    printf("[peek] not implemented!\n");
    return NULL;
}

int getMinPriority(PQueue *pq){
/* return priority of first node in queue; return -1 if queue is empty
 *   args: 
 *       :PQueue *pq - the priority queue to manipulate
 *   returns:
 *       :priority of first node, -1 if empty
 */
    printf("[getMinPriority] not implemented!\n");
    return -999;
}
