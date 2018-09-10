#include "slist.djberenb.h" /* custom interface */

void printList(SListNode *theList){
    while (theList != NULL){
        printf("%d ", theList->data);
        theList = theList->next;
    }
}

int insertAtEnd(SListNode **theList, int data){
    
    if ((*theList) == NULL){
        SListNode *node;
        node = (SListNode *) malloc(sizeof(SListNode));
        node->data = data;
        node->next = NULL;
        return 0;
    }

    while ((*theList)->next != NULL){
        *theList = (* theList)->next;
    }

    SListNode *node;
    node = (SListNode *) malloc(sizeof(SListNode));
    node->data = data;
    node->next = NULL;

    (*theList)->next = node;

    return 0;
}



