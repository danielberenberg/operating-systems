#include "dlist.djberenb.h"
#include "slist.djberenb.h"
#include "stdio.h"

int main(int argc, char *argv[]){
    DListNode *theList;
    SListNode *theSList;
    int i;

    theSList = NULL;
    printf("singly linked list\n");
    for (i=1; i<=10; ++i){
        insertAtEndS(&theSList, i);        
        insertSortedS(&theSList, -1*i);
    }

    printListS(theSList);

    printf("=========================================\n");
    
    printf("doubly linked list\n");
    theList = NULL;
    for (i=1; i<=10; ++i){
        insertAtEnd(&theList, i);        
    }

    insertSorted(&theList,16);
    insertSorted(&theList,5);
    insertSorted(&theList,-1);

    printList(theList);
    printListReverse(theList);
}

