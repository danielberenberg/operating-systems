#include "slist.djberenb.h" /* custom interface */
#include <stdio.h>  /* I/O such as printf */
#include <stdlib.h> /* malloc, free */


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

void createNode(SListNode **node, int data, SListNode *next){
    /*
     * Allocate memory for and populate the fields of a new
     * Linked List node. Intentionally left out of .h as a private API
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

     SListNode *n;
     n = (SListNode *) malloc(sizeof(SListNode));
     n->data = data;
     n->next = next;
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
        createNode(theList, data,NULL);
        return 0;
    }
    
    // the list is more than length 0, create a new tail
    SListNode *curr;  // a dummy variable to help iterate through list  
    curr = *theList;
    while (curr->next != NULL){
        curr = curr->next;
    }
    
    SListNode *node;
    createNode(&node, data, NULL);
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

    SListNode *curr = (*theList);
    SListNode *node;
    if (curr->data > data){
        createNode(&node, data, curr);    
        node->next = curr;
        *theList = node;
        return 0;
    }

    // iterate through the list until insertion location is reached
    while (curr->next != NULL && curr->next->data < data){
        curr = curr->next;
    }

    createNode(&node, data, curr->next);
    curr->next = node;
    return 0;
}


int isInList(SListNode *theList, int data){
   /*
    * Validate that a node carrying data is in theList
    *
    * args:
    *     :(SListNode) *theList - list to search
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

int deleteFromList(SListNode **theList, int data){
    /*
     * delete all occurrences of data from list
     *
     * args:
     *     :(SListNode) **theList - list to search
     *     :(int) data - data to query
     *
     * returns:
     *     :(int) - 0 on successful deletion, 1 otherwise
     */ 

    int d = 1; // indicates deletion or not
    SListNode *curr, *prev; 
    curr = (*theList);
    prev = NULL;

    while (curr != NULL){

        // if a match is found
        if (curr->data == data){ 
            d = 0; // update "deleted" indicator
            
            // if not at the head of the list
            if (prev != NULL){
                // rewire appropriately 
                prev->next = curr->next;
                // get rid of the data
                free(curr);
                // update curr, not prev (since node was deleted)
                curr = prev->next;
            }

            else { // at head of list 

                *theList = (*theList)->next; // reassign head
                free(curr);
                curr = (*theList);
            }
        }

        // no match, keep parsing
        else {
            prev = curr;       // update prev to current
            curr = curr->next; // current becomes next
        }
    }
    return d;
}

