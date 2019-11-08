#include <string.h>
#include <stdlib.h>

#include "red_black_tree.h"

RbtNode * _root;

RbtNode* create_rbtnode(char * id, char * firstname, char * lastname, int birthyear, char gender, int postcode) {
    RbtNode* newnode = (RbtNode *) malloc(sizeof (RbtNode));
    newnode->color = RED;
    newnode->key = id;
    newnode->left = NULL;
    newnode->parent = NULL;
    newnode->right = NULL;
    newnode->voted = 0;
    newnode->data.id = id;
    newnode->data.birthyear = birthyear;
    newnode->data.gender = gender;
    newnode->data.firstname = firstname;
    newnode->data.lastname = lastname;
    newnode->data.postcode = postcode;
    return newnode;
}

void initializeRB() {
    _root = NULL;
}

RbtNode* parentRB(RbtNode* n) {
    if (n != NULL) {
        return n->parent;
    } else {
        return NULL;
    }
}

RbtNode* grantparentRB(RbtNode* n) {
    if (n != NULL) {
        return parentRB(parentRB(n));
    } else {
        return NULL;
    }
}

RbtNode* leftsiblingRB(RbtNode* n) {
    if (n != NULL) {
        return n->left;
    } else {
        return NULL;
    }
}

RbtNode* rightsiblingRB(RbtNode* n) {
    if (n != NULL) {
        return n->right;
    } else {
        return NULL;
    }
}

RbtNode* siblingRB(RbtNode* n) {
    if (n != NULL) {
        RbtNode* p = parentRB(n);

        if (p != NULL) {
            if (n == p->left) {
                return p->right;
            } else {
                return p->left;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

RbtNode* uncleRB(RbtNode* n) {
    return siblingRB(parentRB(n));
}

void RotateLeftRB(RbtNode* n) {
    if (n == NULL) {
        return;
    }

    RbtNode* p = parentRB(n);
    RbtNode* nnew = n->right;

    n->right = nnew->left;
    nnew->left = n;
    n->parent = nnew;

    if (n->right != NULL) {
        n->right->parent = n;
    }

    if (p != NULL) {
        if (n == p->left) {
            p->left = nnew;
        } else if (n == p->right) {
            p->right = nnew;
        }
    }
    nnew->parent = p;
}

void RotateRightRB(RbtNode* n) {
    if (n == NULL) {
        return;
    }
    RbtNode* p = parentRB(n);
    RbtNode* nnew = n->left;

    n->left = nnew->right;
    nnew->right = n;
    n->parent = nnew;

    if (n->left != NULL) {
        n->left->parent = n;
    }

    if (p != NULL) {
        if (n == p->left) {
            p->left = nnew;
        } else if (n == p->right) {
            p->right = nnew;
        }
    }
    nnew->parent = p;
}

RbtNode* searchRB(const char * key) {
    if (_root == NULL) {
        return NULL;
    }

    RbtNode * temp = _root;

    while (temp != NULL) {
        if (strcmp(temp->key, key) == 0) {
            return temp;
        }
        if (strcmp(temp->key, key) < 0) {
            temp = leftsiblingRB(temp);
        }

        if (strcmp(temp->key, key) > 0) {
            temp = rightsiblingRB(temp);
        }
    }

    return NULL;
}

void RepairRB(RbtNode* n) {
    if (parentRB(n) == NULL) {
        n->color = BLACK;
    } else if (parentRB(n)->color == BLACK) {
        return;
    } else if (uncleRB(n) != NULL && uncleRB(n)->color == RED) {
        parentRB(n)->color = BLACK;
        uncleRB(n)->color = BLACK;
        grantparentRB(n)->color = RED;
        RepairRB(grantparentRB(n));
    } else {
        RbtNode* p1 = parentRB(n);
        RbtNode* g1 = grantparentRB(n);

        if (n == p1->right && p1 == g1->left) {
            RotateLeftRB(p1);
            n = n->left;
        } else if (n == p1->left && p1 == g1->right) {
            RotateRightRB(p1);
            n = n->right;
        }


        RbtNode* p2 = parentRB(n);
        RbtNode* g2 = grantparentRB(n);

        if (n == p2->left) {
            RotateRightRB(g2);
        } else {
            RotateLeftRB(g2);
        }
        p2->color = BLACK;
        g2->color = RED;
    }
}

RbtNode* insertRB(char * id, char * firstname, char * lastname, int birthyear, char gender, int postcode) {
    if (_root == NULL) {
        _root = create_rbtnode(id, firstname, lastname, birthyear, gender, postcode);
        
         RepairRB(_root);
        return _root;
    }

    RbtNode * temp = _root;
    RbtNode * n = NULL;

    while (temp != NULL) {
        if (strcmp(temp->key, id) == 0) {
            return NULL;
        }
        if (strcmp(temp->key, id) < 0) {
            if (leftsiblingRB(temp)) {
                temp = leftsiblingRB(temp);
            } else {
                temp->left = create_rbtnode(id, firstname, lastname, birthyear, gender, postcode);
                temp->left->parent = temp;
                n = temp->left;
                break;
            }
        }

        if (strcmp(temp->key, id) > 0) {
            if (rightsiblingRB(temp)) {
                temp = rightsiblingRB(temp);
            } else {
                temp->right = create_rbtnode(id, firstname, lastname, birthyear, gender, postcode);
                temp->right->parent = temp;
                n = temp->right;
                break;
            }
        }
    }

    temp = n;

    RepairRB(temp);

    while (parentRB(temp) != NULL) {
        temp = parentRB(temp);
    }
    _root = temp;

    return n;
}

void cleanupRBTrec(RbtNode * n) {
     if (n != NULL) {
        RbtNode * nl = leftsiblingRB(n);
        RbtNode * nr = rightsiblingRB(n);
        
        cleanupRBTrec(nl);
        cleanupRBTrec(nr);
        
        free(n->data.id);
        free(n->data.firstname);
        free(n->data.lastname);
        free(n);
    }
}

void cleanupRBT() {
    cleanupRBTrec(_root);
}


int _rec_counter ;

void searchVotedRBTrec(RbtNode * n) {
     if (n != NULL) {
        RbtNode * nl = leftsiblingRB(n);
        RbtNode * nr = rightsiblingRB(n);
        
        searchVotedRBTrec(nl);
        searchVotedRBTrec(nr);
        
        if (n->voted) {
            _rec_counter++;
        }
    }
}


int searchVotedRBT() {
    _rec_counter =0 ;
    
    searchVotedRBTrec(_root);
            
    return _rec_counter;
}