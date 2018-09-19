#include "dlist.djberenb.h" /* custom interface */
#include <stdio.h>  /* I/O such as printf */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* string operations such as concatenation */

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
