#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
//here, you must include your own dlist.netid.h
#include "dlist.djberenb.h"


// instructions--do these commands (use your netid):
// gcc -c dlist.netid.c
// gcc -c dlist-new-tests.c
// gcc dlist.netid.o dlist-new-tests.o
//
// and then run the a.out that you get

/*--------------------------------------------------------------------------*/

int numpass;
int numtests = 5;


/*--------------------------------------------------------------------------*/

int findLoops(DListNode *theList) {
  DListNode *prevNode, *listNode;
  int fail, nodeIdx, i;
  DListNode *nodes[64];

  nodeIdx = 0;
  // check for loop, forward

  prevNode = NULL;

  nodeIdx = 0;
  listNode = theList;
  fail = 0;
  while (nodeIdx < 64 && listNode != NULL && ! fail) {
    nodes[nodeIdx] = listNode;
    ++nodeIdx;
    i = 0;
    while (i < nodeIdx-1 && ! fail) {
      if (listNode == nodes[i]) {
        fail = 1;
        printf("i = %d, nodeIdx = %d\n", i, nodeIdx);
        if (prevNode == NULL)
          theList->next = NULL;
        else
          prevNode->next = NULL;
      } else
        ++i;
    }
    if (!fail) {
      prevNode = listNode;
      listNode = listNode->next;
    }
  }
  if (fail)
    printf("ERROR: loop in forward list\n");

  // check for loop, reverse
  prevNode = NULL;
  nodeIdx = 0;
  listNode = theList;
  if (listNode != NULL) {
    while (listNode->next != NULL)
      listNode = listNode->next;
  }

  fail = 0;
  while (nodeIdx < 64 && listNode != NULL && ! fail) {
    nodes[nodeIdx] = listNode;
    ++nodeIdx;
    i = 0;
    while (i < nodeIdx-1 && ! fail) {
      if (listNode == nodes[i]) {
        fail = 1;
        printf("i = %d, nodeIdx = %d\n", i, nodeIdx);
        if (prevNode == NULL)
          theList->prev = NULL;
        else
          prevNode->prev = NULL;
      } else
        ++i;
    }
    if (!fail) {
      prevNode = listNode;
      listNode = listNode->prev;
    }
  }
  if (fail)
    printf("ERROR: loop in reverse list\n");
  return(fail);
}

/*--------------------------------------------------------------------------*/

int checkReverse(DListNode *theList) {
  DListNode *nodes[64];
  int idx, fail;
  DListNode *listNode, *prevNode;

  idx = 0;
  listNode = theList;
  if (listNode == NULL)
    return(0);

  if (listNode->next == NULL) {
    if (listNode->prev != NULL) {
      printf("ERROR: prev pointer is wrong\n");
      return(1);
    }
  }

  prevNode = NULL;
  while (listNode->next != NULL) {
    nodes[idx] = listNode;
    ++idx;
    listNode = listNode->next;
  }

  fail = 0;
  idx = idx - 1;
  listNode = listNode->prev;
  while (! fail && idx >= 0 && listNode != NULL) {
    if (listNode != nodes[idx]) {
      printf("ERROR: prev pointer is wrong\n");
      fail = 1;
    }
    listNode = listNode->prev;
    idx = idx - 1;
  }

  if (idx != -1) {
    printf("ERROR: reverse list is not correct length: %d\n", idx);
    fail = 1;
  }

  return(fail);
}

/*--------------------------------------------------------------------------*/

int checkDList(int *A, DListNode *theList, int numVals, char *testName) {
  DListNode *listNode;
  int i;
  int fail;
  DListNode *prevNode;
  fail = checkReverse(theList);
  fail = findLoops(theList);

  i = 0;
  listNode = theList;
  while (i < numVals && listNode != NULL) {
    if (A[i] != listNode->data) {
      fail = 1;
      printf("ERROR: forward mismatch %d %d in test %s\n", A[i], listNode->data, testName);
    }
    i = i + 1;
    listNode = listNode->next;
  }

  if (i < numVals) {
    printf("ERROR: list too short (expecting length %d) in test %s\n", numVals, testName)
;
    fail = 1;
  }

  if (listNode != NULL) {
    printf("ERROR: list too long (expecting length %d) in test %s\n", numVals, testName);
    fail = 1;
  }

  // now check backwards
  listNode = theList;

  if (listNode != NULL) {
    while (listNode->next != NULL)
      listNode = listNode->next;

    i = numVals-1;
    while (i >= 0 && listNode != NULL) {
      if (A[i] != listNode->data) {
        fail = 1;
        printf("ERROR: backward mismatch %d %d in test %s\n", A[i], listNode->data, testName);
      }
      i = i - 1;
      listNode = listNode->prev;
    }
  } else {
    i = -1;
  }
  if (i != -1) {
    printf("ERROR: in reverse traversal (expecting length %d) in test %s\n", numVals, testName)
;
    fail = 1;
  }

  if (listNode != NULL) {
    printf("ERROR: in reverse traversal (expecting length %d) in test %s\n", numVals, testName);
    fail = 1;
  }

  if ( ! fail )
    printf("%s: OK\n", testName);

  return(fail);
}

/*--------------------------------------------------------------------------*/

int test_one() {
  DListNode *theList;
  int i;
  theList = NULL;
  int A[64];
  int sts, fail;

  fail = 0;

  for (i=0; i<10; ++i) {
    insertAtEnd(&theList, i);
  }

  //printList(theList);
  // should be 0 1 2 3 4 5 6 7 8 9
  A[0] = 0;
  A[1] = 1;
  A[2] = 2;
  A[3] = 3;
  A[4] = 4;
  A[5] = 5;
  A[6] = 6;
  A[7] = 7;
  A[8] = 8;
  A[9] = 9;

  sts = checkDList(A, theList, 10, "test-one-a");
  if (sts != 0)
    fail = 1;

  deleteFromList(&theList, 4);
  A[0] = 0;
  A[1] = 1;
  A[2] = 2;
  A[3] = 3;
  A[4] = 5;
  A[5] = 6;
  A[6] = 7;
  A[7] = 8;
  A[8] = 9;

  sts = checkDList(A, theList, 9, "test-one-b");
  if (sts != 0)
    fail = 1;

  for (i=0; i<10; ++i) {
    deleteFromList(&theList, i);
  }
  sts = checkDList(A, theList, 0, "test-one-c");
  if (sts != 0)
    fail = 1;
  return(fail);
}

/*--------------------------------------------------------------------------*/

int test_two() {
  DListNode *theList;
  int i;
  int A[64];
  int sts;
  int fail = 0;

  theList = NULL;
    
  insertSorted(&theList, 7);
  insertSorted(&theList, 5);
  insertSorted(&theList, 9);
  insertAtEnd(&theList, 9);
  insertSorted(&theList, 5);
    
  if (isInList(theList, 1)) {
    printf("ERROR: test-two-a don't expect 1 in list\n");
    fail = 1;
  }
  if (! isInList(theList, 5)) {
    printf("ERROR: test-two expect 5 in list\n");
    fail = 1;
  }
  if (! isInList(theList, 9)) {
    printf("ERROR: test-two expect 9 in list\n");
    fail = 1;
  }
  
  deleteFromList(&theList, 9);
  if (isInList(theList, 9)) {
    printf("ERROR: test-two don't expect 9 in list\n");
    fail = 1;
  }
  int d;
  for (i=0; i<10; ++i){
    d = deleteFromList(&theList, i);
  }

  sts = checkDList(A, theList, 0, "test-two");
  if (sts != 0)
    fail = 1;
  return(fail);
}

/*---------------------------------------------------------------------------*/

int test_three() {
  DListNode *theList;
  int i;
  int A[64];
  int sts, fail;

  fail = 0;

  theList = NULL;

  for (i=1; i<=10; ++i) {
    if (i%2 == 0)
      //insertSorted(&theList, i);
      insertAtEnd(&theList, i);
    else
      //insertSorted(&theList, 10-i);
      insertAtEnd(&theList, 10-i);
  }

  //printList(theList);
  // expect 9 2 7 4 5 6 3 8 1 10

  A[0] = 9;
  A[1] = 2;
  A[2] = 7;
  A[3] = 4;
  A[4] = 5;
  A[5] = 6;
  A[6] = 3;
  A[7] = 8;
  A[8] = 1;
  A[9] = 10;
  sts = checkDList(A, theList, 10, "test-three-a");
  if (sts)
    fail = 1;

  for (i=1; i<=10; ++i) {
    if (i%2 == 0)
      deleteFromList(&theList, i);
    else
      deleteFromList(&theList, 10-i);
  }
  // printList(theList);
  // expect empty list
  sts = checkDList(A, theList, 0, "test-three-b");
  if (sts)
    fail = 1;
  return(fail);
}

/*------------------------------------------------------------------------*/

int test_four() {
  DListNode *theList;
  int i;
  int A[64];
  int sts, fail;

  fail = 0;

  theList = NULL;

  for (i=1; i<=10; ++i) {
    if (i<5)
      insertSorted(&theList, 1);
    else
      insertSorted(&theList, 10);
  }
  //printList(theList);
  // 1 1 1 1 10 10 10 10 10 10

  A[0] = 1;
  A[1] = 1;
  A[2] = 1;
  A[3] = 1;
  A[4] = 10;
  A[5] = 10;
  A[6] = 10;
  A[7] = 10;
  A[8] = 10;
  A[9] = 10;
  sts = checkDList(A, theList, 10, "test-four-a");
  if (sts)
    fail = 1;

  deleteFromList(&theList, 1);
  //printList(theList);
  // expect 10 10 10 10 10 10
  A[0] = 10;
  A[1] = 10;
  A[2] = 10;
  A[3] = 10;
  A[4] = 10;
  A[5] = 10;
  sts = checkDList(A, theList, 6, "test-four-b");
  if (sts)
    fail = 1;

  deleteFromList(&theList, 2);
  //printList(theList);
  // expect 10 10 10 10 10 10
  sts = checkDList(A, theList, 6, "test-four-c");
  if (sts)
    fail = 1;

  deleteFromList(&theList, 10);
  // expect empty list
  sts = checkDList(A, theList, 0, "test-four-d");
  if (sts)
    fail = 1;

  return(fail);
}

/*------------------------------------------------------------------*/

int test_five() {
  DListNode *theList;
  int i;
  int A[64];
  int sts, fail;

  fail = 0;

  theList = NULL;
  insertSorted(&theList, 5);
  insertSorted(&theList, 1);
  insertSorted(&theList, 9);
  insertSorted(&theList, 3);
  insertSorted(&theList, 8);
  insertSorted(&theList, 4);
  insertSorted(&theList, 6);
  insertSorted(&theList, 2);
  insertSorted(&theList, 7);
  insertSorted(&theList, 1);
  insertSorted(&theList, 5);
  insertSorted(&theList, 9);
  //printList(theList);
  // list should be 1 1 2 3 4 5 5 6 7 8 9 9

  A[0] = 1;
  A[1] = 1;
  A[2] = 2;
  A[3] = 3;
  A[4] = 4;
  A[5] = 5;
  A[6] = 5;
  A[7] = 6;
  A[8] = 7;
  A[9] = 8;
  A[10] = 9;
  A[11] = 9;
  sts = checkDList(A, theList, 12, "test-five-a");
  if (sts)
    fail = 1;

  for (i=1; i<10; ++i)
    deleteFromList(&theList, i);
  sts = checkDList(A, theList, 0, "test-five-b");
  if (sts)
    fail = 1;
  return(fail);
}

/*------------------------------------------------------------------*/

void handler(int signum) {
  printf("ERROR: got a signal %d\n", signum);
  printf("\n");
  printf("number of passing tests = %d / %d\n", numpass, numtests);
  exit(8);
}

/*------------------------------------------------------------------*/

int main(int argc, char *argv[]) {
  int sts;
  char *home;
  char str[64];
  struct sigaction action;

  numpass = 0;

  memset(&action, 0, sizeof(struct sigaction));

  action.sa_handler = handler;

  sigaction(SIGSEGV, &action, NULL);

  numpass = 0;
    
  sts = test_one();
  if (sts == 0)
    ++numpass;

  sts = test_two();
  if (sts == 0)
    ++numpass;

  sts = test_three();
  if (sts == 0)
    ++numpass;

  sts = test_four();
  if (sts == 0)
    ++numpass;

  sts = test_five();
  if (sts == 0)
    ++numpass;

  printf("\n");
  printf("number of passing tests = %d / %d\n", numpass, numtests);
}

