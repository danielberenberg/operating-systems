#include "slist.djberenb.h" /* custom interface */

void printList(SListNode *theList){
    /* Print a linked list
     * args:
     *     :(**theList) - list to print
     *
     * returns:
     *     :void
     */
    
    // iterate through the list
    while (theList != NULL){
        printf("%d ", theList->data);
        theList = theList->next;
    }

    // print newline at end of list
    printf("\n");
}

int insertAtEnd(SListNode **theList, int data){
    /* Append a node to the end of a linked list
     * args:
     *     :**theList (SListNode) - the head of the linked list
     *     :data (int) - the data the appended node will carry
     * returns:
     *     :(int) - 0 on successful appension
     */

    // if the list has no head, generate one and return
    if ((*theList) == NULL){
        SListNode *node;
        node = (SListNode *) malloc(sizeof(SListNode));
        node->data = data;
        node->next = NULL;
        *theList = node;
        return 0;
    }
    
    // the list is more than length 0, create a new tail
    SListNode *curr;  // a dummy variable to help iterate through list  
    curr = *theList;
    while (curr->next != NULL){
        curr = curr->next;
    }

    SListNode *node;
    node = (SListNode *) malloc(sizeof(SListNode));
    node->data = data;
    node->next = NULL;
    curr->next = node;

    return 0;
}



