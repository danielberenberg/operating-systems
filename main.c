#include "dlist.djberenb.h"
#include "slist.djberenb.h"
#include "stdio.h"

void test_1D() {
  DListNode *theList;
  int i;
  theList = NULL;
    for (i=1; i<=10; ++i) {
    insertAtEnd(&theList, i);
}
  printList(theList);
  // should get 1 2 3 4 5 6 7 8 9 10
  for (i=20; i>=0; i=i-2) {
    if ( isInList(theList, i) ) {
      deleteFromList(&theList, i);
    } // 9-17-18 I forgot this bracket
}
  printList(theList);
  printf("should get 1 3 5 7 9\n");
  // should get 1 3 5 7 9
}
void test_2D() {
  DListNode *theList;
  int i;
  theList = NULL;
  for (i=1; i<=10; ++i) {
    if (i%2 == 0)
      insertSorted(&theList, i);
    else
      insertSorted(&theList, 10-i);
}
  printList(theList);
  // should get 1 2 3 4 5 6 7 8 9 10
  for (i=1; i<=9; ++i) {
    if (i%2 == 0){
      deleteFromList(&theList, i);
    }
    else {
      deleteFromList(&theList, 10-i);
    }
    printList(theList);
  }
  printList(theList);
  printf("should get 10\n");
  // should get 10
}
void test_3D() {
  DListNode *theList;
  int i;
  theList = NULL;
  for (i=1; i<=10; ++i) {
    if (i < 5)
      insertSorted(&theList, 1);  // 9-17-18 I changed this
    else
      insertSorted(&theList, 10);
  }
  printList(theList);

// should get 1 1 1 1 10 10 10 10 10 10
  deleteFromList(&theList, 1);
  printList(theList);
  // should get 10 10 10 10 10 10
  deleteFromList(&theList, 2);
  printList(theList);
  // should get 10 10 10 10 10 10
  deleteFromList(&theList, 10);
  printList(theList);
  printf("nothing\n");
  // should get nothing printed for the list
}
void test_4D() {
  DListNode *theList; // 9-17-18 I forgot a semicolon here
  int i;
  theList = NULL;
  for (i=1; i<=10; ++i) {
    if (i%2 == 0)
      insertSorted(&theList, i);
    else
      insertSorted(&theList, 10-i);
}
  printList(theList);
  // should get 1 2 3 4 5 6 7 8 9 10
  printListReverse(theList);
  // should get 10 9 8 7 6 5 4 3 2 1
}
int main(int argc, char *argv[]){
    printf("test1\n");
    test_1D();
    printf("test2\n");
    test_2D();
    printf("test3\n");
    test_3D();
    printf("test4\n");
    test_4D();
}

