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

     // iterate through the list
     while (theList != NULL){
        printf("%d ", theList->data);
        theList = theList->next;
     }

     // print newline at end of list
     printf("\n");
}

void printListReverse(DListNode *theList){
    /* Print a doubly linked list in reverse
     * args:
     *     :DListNode (*theList) - list to print
     * returns:
     *     :void
     */         
     
     // iterate through the list
     while(theList->next != NULL){
        theList = theList->next;
     }

     while(theList != NULL){
        printf("%d ", theList->data);
        theList = theList->prev;
     }

     printf("\n");
      
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
    
    if ((*theList) == NULL){
        // create a node with no tail
        createNode(theList, data, NULL, NULL);
        return 0;
    }

    DListNode *curr = *theList;
    while (curr->next != NULL){
        curr = curr->next;
    }

    DListNode *node;
    createNode(&node, data,NULL, curr); 
    curr->next = node;

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

    DListNode *curr = (*theList);
    DListNode *node;
    if (data < curr->data){
        createNode(&node, data, curr, NULL);
        *theList = node;
        return 0;
    }

    while (curr->next != NULL){
        if (data < curr->next->data){
            createNode(&node, data, curr->next, curr);
            curr->next->prev = node;
            curr->next = node;
            return 0;
        }
        curr = curr->next;
    }
    
    createNode(&node, data, NULL, curr);
    curr->next = node;
    return 0; 
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
     // iterate through the list
     while (theList != NULL){
        if (theList->data == data){ return 1; }
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
    
    int d = 1; // set deletion marker to "no successful deletion"
    DListNode *curr, *prev;
    curr = (*theList);
    prev = NULL;
    if (curr == NULL){
        return 1; 
    }
    if (curr->next == NULL && curr->data == data){
       *theList = NULL; 
       free(curr);
       return 0;
    }

    while (curr != NULL){
        if (curr->data == data){
            d = 0;
            if (prev != NULL){
                // rewire the nodes
                if (curr->next != NULL){
                    prev->next = curr->next;
                    curr->next->prev = prev;
                }
                else {
                    prev->next = NULL;
                }

                // get rid of the data     
                free(curr);
                // update curr, not prev
                curr = prev->next;
            }
            else { // at the head of the list 
                if (curr->next != NULL){ curr->next->prev = NULL; }
                *theList = (*theList)->next; // reassign head
                free(curr);
                curr = (*theList);
            }
        }
        else {
            prev = curr;       // prev --> current
            curr = curr->next; // curr bexcomes next
        }
    }
    return d;
}
