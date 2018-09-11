#include "slist.djberenb.h"

int main(int argc, char *argv[]){
    SListNode *theList;
    int i;
    int j;
    
    printf("declared some vars\n");
    theList = NULL;
    for (i=1; i<=10; ++i){
        insertAtEnd(&theList, i);        
    }
    insertSorted(&theList, -1);
    j = 5;
    printf("%d is in list %d\n",j, isInList(theList, j));
    insertSorted(&theList, 120);
    insertSorted(&theList,5);
    j = 15;
    printf("%d is in list %d\n",j, isInList(theList, j));
    printf("pre-deletion: ");
    printList(theList);
    printf("deleted: ");
    deleteFromList(&theList, 2);
    printf("%d ", 2);
    deleteFromList(&theList, -1);
    printf("%d ", -1);
    deleteFromList(&theList, 5);
    printf("%d ", 5);
    deleteFromList(&theList, 120);
    printf("%d ", 120);
    deleteFromList(&theList, 111);
    printf("%d\n", 111);
    printf("post-deletion: ");
    printList(theList);
}

