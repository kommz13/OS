#ifndef LIST_H
#define LIST_H

typedef struct ListNode {
    char * data;
    struct ListNode * next;
} ListNode;

// ----------------------------

ListNode * initializeL();

void cleanupL(ListNode * _head);

int containsL(ListNode * _head, char * flag);

void insertL(ListNode ** _head, char * data);

char * peekL(ListNode * _head);

int countL(ListNode * _head);

void printL(ListNode * _head);

void destroyL(ListNode * _head);

#endif
