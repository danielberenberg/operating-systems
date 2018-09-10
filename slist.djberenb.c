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
    /*
     * Insert a node into a linked list such that the node
     * is located at the first occurrence of the value of 
     * data being greater than the current node's data value
     *
     * E.g for a list [1 2 3 4 6]
     *
     * insertSorted([1 2 3 4 6],5) would result in a list
     *  [1 2 3 4 5 6]
     *
     *  args:
     *      :**theList - linked list into which data will be inserted
     *      :data (int) - data to insert
     *
     *  returns:
     *      :(int) - 0 on successful insertion
     */

    // list has no head, just create a length 1 list
    if ((*theList) == NULL){
        return insertAtEnd(theList, data);
    }

    SListNode *prev = NULL;
    SListNode *curr = (*theList);
    // iterate through the list until insertion location is reached
    while (prev->next != NULL && prev->data < data){
        prev = curr;
        curr = curr->next;
    }
    
    SListNode *node;

    createNode(&node, data);
    node->next = curr; 
    printf("insertSorted\n");

    return 0;
}

