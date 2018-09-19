typedef struct dlistStruct {
    int data;
    struct dlistStruct *next;
    struct dlistStruct *prev;
} DListNode;


void printList(DListNode *theList);
/* print the list from head to tail */

void printListReverse(DListNode *theList);
/* print the list from tail to head */

int insertAtEnd(DListNode **theList, int data);
/* return zero if successful */

int insertSorted(DListNode **theList, int data);
/* return zero if successful */

int isInList(DListNode *theList, int data);
/* return nonzero if data is found in the list; 0 otherwise */

int deleteFromList(DListNode **theList, int data);
/* if no data in the list, do nothing and return nonzero value;
 * other wise delete all listnode matching the data from the list
 * and return zero */
