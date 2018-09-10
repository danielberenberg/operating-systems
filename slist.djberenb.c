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

void createNode(SListNode **node, int data){
    /*
     * Allocate memory for and populate the fields of a new
     * Linked List node
     *
     * args:
     *     :**node (SListNode) - memory location for this new node
     *     :data (int) - the data that n will carry
     *
     * returns:
     *     :(void)
     */

     SListNode *n;
     n = (SListNode *) malloc(sizeof(SListNode));
     n->data = data;
     n->next = NULL;
     *node = n; 
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
        printf("generating head\n");
        createNode(theList, data);
        return 0;
    }
    
    // the list is more than length 0, create a new tail
    SListNode *curr;  // a dummy variable to help iterate through list  
    curr = *theList;
    while (curr->next != NULL){
        curr = curr->next;
    }
    
    SListNode *node;
    createNode(&node, data);
    curr->next = node;

    return 0;
}

int insertSorted(SListNode **theList, int data){

    // list has no head, just create a length 1 list
    if ((*theList) == NULL){
        return insertAtEnd(theList, data);
    }

    SListNode *curr;
    curr = (* theList);
    // iterate through the list until insertion location is reached
    while (curr->next != NULL || curr->next->data < data){
        curr = curr->next;
    }
    
    //SListNode *node;
    //node = (S

}

