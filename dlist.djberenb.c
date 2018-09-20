#include "dlist.djberenb.h" /* custom interface */
#include <stdio.h>  /* I/O such as printf */
#include <stdlib.h> /* malloc, free */

void printList(DListNode *theList){
    /* Print a doubly linked list
     * args:
     *     :DListNode (*theList) - list to print
     *
     * returns:
     *     :void
     */
    
    DListNode *head = theList;
    DListNode *tail = head->prev;
    DListNode *curr = head;
    tail->next = NULL;  // flatten the list into a line
    while (curr != NULL){
        printf("%d ", curr->data);
        curr = curr->next;
    }

    // print newline at end of list
    printf("\n");
    tail->next = head; // conjoin the list again 
}

void printListReverse(DListNode *theList){
    /* Print a doubly linked list in reverse
     * args:
     *     :DListNode (*theList) - list to print
     * returns:
     *     :void
     */    
     
     DListNode *head = theList;
     DListNode *tail = head->prev;
     DListNode *curr = tail;
     head->prev = NULL; // flatten the list into a line
     while (curr != NULL){
        printf("%d ", curr->data);
        curr = curr->prev;
     }

     printf("\n");
     head->prev = tail; // conjoin the list
}

void createNode(DListNode **node, int data, DListNode *next, DListNode *prev){
    /*
     * Allocate memory for and populate the fields of a new
     * Doubly Linked List node. Intentionally left out of .h as a private API
     *
     * args:
     *     :**node (SListNode) - memory location for this new node
     *     :data (int) - the data that n will carry
     *     :SListNode *next - pointer to the next node to support insertion operations; 
     *                        pass NULL for no neighbor
     *
     * returns:
     *     :(void)
     */

     DListNode *n;
     n = (DListNode *) malloc(sizeof(DListNode));
     n->data = data;
     n->next = next;
     n->prev = prev;
     *node = n; 
}

int insertAtEnd(DListNode **theList, int data){
    /* Append a node to the end of a doubly linked list. Implemented so that
     * the resultant DLL is circular so that   head <- ... <- tail <- head <- ...
     * for easy access to the tail of the list
     *
     * args:
     *     :**theList (DListNode) - the head of the linked list
     *     :data (int) - the data the appended node will carry
     * returns:
     *     :(int) - 0 on successful appension
     */
    
    DListNode *created; 
    // if the list has no head, generate one and return a list with a node pointing to itself
    // as both its own previous and next node
    if ((*theList) == NULL){
        createNode(&created, data,NULL, NULL);
        created->prev = created->next = created;
        *theList = created;

        return 0;
    }

    DListNode *head, *tail;
    head = *theList;
    tail = head->prev;
    createNode(&created, data, head, tail);

    tail->next = head->prev = created;
    return 0;
}

int insertSorted(DListNode **theList, int data){
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
    
    // populate a null list
    if ((*theList) == NULL){
        return insertAtEnd(theList, data);
    }

    // initialize some placeholder variables and dummies
    DListNode *head, *tail, *curr, *node;
    head = (*theList);
    tail = head->prev;
    curr = head;
    
    // iterate through the list until reaching the tail
    while (curr != tail){
        if (data < curr->data){
        
           // placing node N containing `data` between two neighbors A and C (current)
           createNode(&node, data, curr, curr->prev); // logically wire up data to point forward to C and backward to A
           curr->prev->next = node; // rewire A to point towards N 
           curr->prev = node; // rewire C to link back to N

           if (curr == head){
               *theList = node; // reassign the head if updating the first element 
           }
           return 0;
        }
        curr = curr->next;
    }
    // reached the tail means to append to the back of the list 
    return insertAtEnd(theList,data); 
}

int isInList(DListNode *theList, int data){
   /*
    * Validate that a node carrying data is in theList
    *
    * args:
    *     :(DListNode) *theList - list to search
    *     :(int) data - data to query
    *
    * returns:
    *     :(int) - 1 if data is contained, 0 otherwise
    */ 
    
    while (theList != NULL){
       if (theList->data == data){
           return 1; 
       }
       theList = theList->next;
    }
    return 0;
}

int deleteFromList(DListNode **theList, int data){
    /*
     * delete all occurrences of data from list
     *
     * args:
     *     :(DListNode) **theList - list to search
     *     :(int) data - data to query
     *
     * returns:
     *     :(int) - 0 on successful deletion, 1 otherwise
     */ 

    int d = 1; // indicates deletion or not
    DListNode *curr, *temp, *tail, *head; 
    curr = head = (*theList);
    tail = head->prev;

    while (curr != tail){

        // if a match is found
        if (curr->data == data){ 
            curr->prev->next = curr->next; // rewire appropriately 
            if (curr == head){ 
                // get rid of the data
                head = curr->next;
                head->prev = curr->prev;
                *theList = head;
                curr = head;
                free(curr);
            }
            else { 
                temp = curr->next;
                free(curr);
                curr = temp;
            }
            d = 0; // update deleted indicator
        }
        else{ curr = curr->next; } // current becomes next }
    }
    return d;
}
