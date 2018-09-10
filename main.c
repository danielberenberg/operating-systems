#include "slist.djberenb.h"

int main(int argc, char *argv[]){
    SListNode *theList;
    int i;
    
    printf("declared some vars\n");
    theList = NULL;
    for (i=1; i<=10; ++i){
        insertAtEnd(&theList, i);        
    }
    printf("passed 4 loop\n");
    printList(theList);
}
