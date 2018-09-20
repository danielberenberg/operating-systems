typedef struct slistStruct {
    int data;
    struct slistStruct *next;
} SListNode;

void printListS(SListNode *theList);
/* print the list items from head to tail */

int insertAtEndS(SListNode **theList, int data);
/* return zero if successful */

int insertSortedS(SListNode **theList, int data);
/* return zero if successful */

int isInListS(SListNode *theList, int data);
/* return non-zero value if the value is in the list; otherwise return zero */

int deleteFromListS(SListNode **theList, int data);
/* if the value isn't in the list, then do nothing and return non-zero value;
 * otherwise deleta all listnodes matching the data from the list and return zero */

