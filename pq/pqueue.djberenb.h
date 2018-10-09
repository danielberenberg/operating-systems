typedef struct {
    char name[16];
} DataNode;

typedef struct PQN {
    int priority;
    void *data;
    struct PQN *next;
} PQueueNode;

typedef struct {
    PQueueNode *head;
    PQueueNode *tail;
} PQueue;

int enqueue(PQueue *pq, int priority, void *data);
/* put node in queue in priority order;
 *  - in case of tie in priority, input node is placed at
 *    the end of the sequence of nodes with the spec. priority */

void *dequeue(PQueue *pq);
/* return NULL if queue is empty; otherwise return the data (the DataNode)
 * from the first node in the queue (since the queue is maintained in priority 
 * order (from lowest to highest) this will be the data from the node having
 * the smallest priority value
 */

int printQueue(PQueue *pq);
// print queue from head to tail; this function should always return 0 

void *peek(PQueue *pq);
// return the first node in the queue, but do not remove it; return NULL if queue is empty

int getMinPriority(PQueue *pq);
// return priority of first node in queue; return -1 if queue is empty
