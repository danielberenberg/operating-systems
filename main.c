#include "dlist.djberenb.h"
#include "stdio.h"

int main(int argc, char *argv[]){
    DListNode *theList;
    int i;
    
    printf("declared some vars\n");
    theList = NULL;
    for (i=1; i<=10; ++i){
        insertAtEnd(&theList, i);        
    }

    printList(theList);
    printListReverse(theList);
    printf("=========================================\n");
}

