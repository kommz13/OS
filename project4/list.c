#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

ListNode * initializeL() {
    ListNode * _head = NULL;
    return _head;
}

void cleanupL(ListNode * _head) {

    ListNode * horizontalPointer = _head;

    if (horizontalPointer == NULL) {
        return;
    } else {
        while (horizontalPointer != NULL) {
            ListNode * temp = horizontalPointer;
            horizontalPointer = horizontalPointer->next;

            free(temp);
        }
    }
}

int containsL(ListNode * _head, char * flag) {
    ListNode * horizontalPointer = _head;

    if (horizontalPointer == NULL) {
        return 0;
    } else {
        while (horizontalPointer != NULL) {
            if (strcmp(horizontalPointer->data, flag) == 0) {
                return 1;
            }
            horizontalPointer = horizontalPointer->next;
        }
    }
    return 0;
}

ListNode * create_listnode(char * data) {
    ListNode* newnode = (ListNode *) malloc(sizeof (ListNode));
    newnode->data = data;
    newnode->next = NULL;
    return newnode;
}

void insertL(ListNode ** _head, char * data) {
    ListNode * horizontalPointer = *_head;

    if (horizontalPointer == NULL) {
        horizontalPointer = create_listnode(data);
        *_head = horizontalPointer;
    } else {
        while (horizontalPointer != NULL) {
            if (horizontalPointer->data == data) {
                break;
            } else {
                if (horizontalPointer->next) {
                    horizontalPointer = horizontalPointer->next;
                } else {
                    horizontalPointer->next = create_listnode(data);
                    horizontalPointer = horizontalPointer->next;
                    break;
                }
            }
        }
    }
}

int countL(ListNode * _head) {
    int n = 0;
    ListNode * horizontalPointer = _head;

    if (horizontalPointer == NULL) {
        return 0;
    } else {
        while (horizontalPointer != NULL) {
            n++;
            horizontalPointer = horizontalPointer->next;
        }
    }
    return n;
}

void printL(ListNode * _head) {
    ListNode * horizontalPointer = _head;

    if (horizontalPointer == NULL) {
        return;
    } else {
        while (horizontalPointer != NULL) {
            printf("%s,", horizontalPointer->data);
            horizontalPointer = horizontalPointer->next;
        }
    }
}

void destroyL(ListNode * _head) {
    cleanupL(_head);
}

char * peekL(ListNode * _head) {
    if (_head == NULL) {
        return NULL;
    } else {
        return _head->data;
    }
}