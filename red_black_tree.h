#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H
#include <stdbool.h>
#include <stddef.h>
#define BLACK 0
#define RED 1
#define Color unsigned char

typedef struct RbtData {
    char * id;
    char * firstname;
    char * lastname;
    int birthyear;
    char gender;
    int postcode;
} RbtData;

typedef struct RbtNode {
    struct RbtNode* parent;
    struct RbtNode* left;
    struct RbtNode* right;
    Color color;
    char * key;

    RbtData data;
    bool voted;
} RbtNode;

void initializeRB();

RbtNode* parentRB(RbtNode* n);

RbtNode* leftsiblingRB(RbtNode* n);

RbtNode* rightsiblingRB(RbtNode* n);

RbtNode* grantparentRB(RbtNode* n);

RbtNode* siblingRB(RbtNode* n);

RbtNode* uncleRB(RbtNode* n);

void RotateLeftRB(RbtNode* n);

void RotateRightRB(RbtNode* n);

RbtNode* searchRB(const char * key);

RbtNode* insertRB(char * id, char * firstname, char * lastname, int birthyear, char gender, int postcode);

void cleanupRBT();

int searchVotedRBT();

#endif

