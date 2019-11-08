#include <stdio.h>
#include <stdlib.h>

#include "red_black_tree.h"
#include "listlist.h"

struct PostNode * _head;

typedef struct ListNode {
    struct RbtNode * rbtNode;
    struct ListNode * next;

} ListNode;

typedef struct PostNode {
    int data;
    struct ListNode * up;
    struct PostNode * next;
} PostNode;

void initializeLL() {
    _head = NULL;
}

void cleanupLL() {

    PostNode * horizontalPointer = _head;

    if (horizontalPointer == NULL) {
        return;
    } else {
        while (horizontalPointer != NULL) {
            PostNode * temp = horizontalPointer;
            horizontalPointer = horizontalPointer->next;


            ListNode* verticalNode = temp->up;

            if (verticalNode != NULL) {
                while (verticalNode != NULL) {
                    ListNode * temp2 = verticalNode;
                    verticalNode = verticalNode->next;

                    free(temp2);
                }
            }

            free(temp);
        }
    }
}

ListNode * create_listnode(RbtNode * rbtNode) {
    ListNode* newnode = (ListNode *) malloc(sizeof (ListNode));
    newnode->rbtNode = rbtNode;
    newnode->next = NULL;
    return newnode;
}

PostNode * create_postnode(int postcode) {
    PostNode* newnode = (PostNode *) malloc(sizeof (PostNode));
    newnode->up = NULL;
    newnode->next = NULL;
    newnode->data = postcode;
    return newnode;
}

void insertLL(RbtNode * rbtNode) {
    PostNode * horizontalPointer = _head;

    if (horizontalPointer == NULL) {
        horizontalPointer = create_postnode(rbtNode->data.postcode);
        _head = horizontalPointer;
    } else {
        while (horizontalPointer != NULL) {
            if (horizontalPointer->data == rbtNode->data.postcode) {
                break;
            } else {
                if (horizontalPointer->next) {
                    horizontalPointer = horizontalPointer->next;
                } else {
                    horizontalPointer->next = create_postnode(rbtNode->data.postcode);
                    horizontalPointer = horizontalPointer->next;
                    break;
                }
            }
        }
    }

    ListNode* verticalNode = horizontalPointer->up;

    if (verticalNode == NULL) {
        horizontalPointer->up = create_listnode(rbtNode);
    } else {
        ListNode* n = create_listnode(rbtNode);
        n->next = horizontalPointer->up;
        horizontalPointer->up = n;
    }
}

void votedPercentLL() {
    PostNode * horizontalPointer = _head;

    if (horizontalPointer == NULL) {
        return;
    } else {
        while (horizontalPointer != NULL) {
            float x = 0, y = 0;

            ListNode* verticalNode = horizontalPointer->up;

            if (verticalNode != NULL) {
                while (verticalNode != NULL) {
                    if (verticalNode->rbtNode->voted) {
                        x++;
                        y++;
                    } else {
                        y++;
                    }

                    verticalNode = verticalNode->next;
                }
            }

            float ratio = x / y;

            printf("postcode: %d  ratio: %f \n", horizontalPointer->data, ratio * 100);

            horizontalPointer = horizontalPointer->next;
        }
    }
}

void votePostCodeLL(int pc) {
    PostNode * horizontalPointer = _head;

    if (horizontalPointer == NULL) {
        return;
    } else {
        while (horizontalPointer != NULL) {
            if (horizontalPointer->data == pc) {
                int x = 0;

                ListNode* verticalNode = horizontalPointer->up;

                if (verticalNode != NULL) {
                    while (verticalNode != NULL) {
                        if (verticalNode->rbtNode->voted) {
                            x++;
                        }

                        verticalNode = verticalNode->next;
                    }
                }

                printf("IN postcode: %d  VOTERS ARE : %d \n", horizontalPointer->data, x);
            }
            horizontalPointer = horizontalPointer->next;
        }
    }
}

ListNode * searchLL(int postcode) {
    return 0;
}