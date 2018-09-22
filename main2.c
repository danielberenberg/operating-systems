#include "dlist.djberenb.h"
#include "slist.djberenb.h"
#include "stdio.h"

void test_1S() {
  SListNode *theList;
  int i;
  theList = NULL;
  for (i=1; i<=10; ++i) {
    insertAtEndS(&theList, i);
  }
  printListS(theList);
  // should get 1 2 3 4 5 6 7 8 9 10
   for (i=20; i>=0; i=i-2) {
    if ( isInListS(theList, i) )
      deleteFromListS(&theList, i);
  }
  printListS(theList);
  printf("should get 1 3 5 7 9\n");
  // should get 1 3 5 7 9
}
void test_2S() {
  SListNode *theList;
  int i;
  theList = NULL;
  for (i=1; i<=10; ++i) {
    if (i%2 == 0)
      insertSortedS(&theList, i);
    else
      insertSortedS(&theList, 10-i);
}
  printListS(theList);
  // should get 1 2 3 4 5 6 7 8 9 10
  for (i=1; i<=9; ++i) {
    if (i%2 == 0)
      deleteFromListS(&theList, i);
    else
      deleteFromListS(&theList, 10-i);
  }
  printListS(theList);
  printf("Should get 10\n");
  // should get 10
}
void test_3S() {
  SListNode *theList;
  int i;
  theList = NULL;
  for (i=1; i<=10; ++i) {
    if (i < 5)
      insertSortedS(&theList, 1); // note I changed this on 9-7-18
    else
      insertSortedS(&theList, 10);
}
  printListS(theList);
  // should get 1 1 1 1 10 10 10 10 10 10
  deleteFromListS(&theList, 1);
  printListS(theList);
  // should get 10 10 10 10 10 10
  deleteFromListS(&theList, 2);

printListS(theList);
  // should get 10 10 10 10 10 10
  printf("should get 10 10 10 10 10 10\n");
  deleteFromListS(&theList, 10);
  printListS(theList);
  printf("nothing\n");
  // should get nothing printed for the list
}

int main(int argc, char *argv[]){
    printf("test1\n");
    test_1S();
    printf("test2\n");
    test_2S();
    printf("test3\n");
    test_3S();
}
