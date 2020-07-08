#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "functions.h"
#include "prime.h"
#include "bloom_filter.h"
#include "red_black_tree.h"
#include "listlist.h"

char * _logfile;
int _numupdates;

void initialize(char * logfile, int numupdates) {
    _logfile = logfile;
    _numupdates = numupdates;

    initializeRB();


    printf("App initialized \n");
}

void createDataStructures() {
    printf("Structures created \n");
}

void loadfile(char * filename) {
    printf("Input file loaded\n");

    FILE * fp = fopen(filename, "r");

    int N = 0;

    char buffer[1000];

    while (fgets(buffer, sizeof (buffer), fp) != NULL) {
        N++;
    }

    fclose(fp);

    int bfsize = 3 * N;

    while (!is_prime(bfsize)) {
        bfsize++;
    }

    initializeBF(bfsize * 8);

    fp = fopen(filename, "r");

    while (fgets(buffer, sizeof (buffer), fp) != NULL) {
        char * firsttoken = strtok(buffer, " ");

        insertBF(firsttoken);

        // DW112135  RICHMOND ROMAN 24  M 45675
        char * a = strtok(NULL, " ");
        char * b = strtok(NULL, " ");
        char * c = strtok(NULL, " ");
        char * d = strtok(NULL, " ");
        char * e = strtok(NULL, " ");

        char *copyid = strdup(firsttoken);
        char *copyfirstname = strdup(a);
        char *copylastname = strdup(b);

        int birthyear = atoi(c);
        char gender = d[0];
        int postcode = atoi(e);

        RbtNode* r = insertRB(copyid, copyfirstname, copylastname, birthyear, gender, postcode);

        if (r != NULL) {
            insertLL(r);
        } else {
            free(copyid);
            free(copyfirstname);
            free(copylastname);
        }
    }

    fclose(fp);

}

void cleanUpDataStructures() {
    printf("Structures cleaned\n");

    cleanupBF();

    cleanupRBT();

    cleanupLL();
}

void lbf(char * key) {
    printf("executing function lbf for key: %s \n", key);

    int result = testBF(key);

    if (result == 1) {
        printf("# %s key POSSIBLY-IN REGISTRY \n", key);
    } else {
        printf("# %s key Not-in-LBF \n", key);
    }
}

void lrb(char * key) {
    printf("executing function lrb for key: %s \n", key);

    RbtNode* node = searchRB(key);

    if (node == NULL) {
        printf("# %s key NOT-IN-RBT \n", key);
    } else {
        RbtData data = node->data;

        printf("# %s key FOUND-IN-RBT \n ID: %s %s %s %c %d %d \n",key, data.id, data.firstname, data.lastname, data.gender, data.birthyear, data.postcode);
    }
}

void ins_record(char * id, char * firstname, char * lastname, int birthyear, char gender, int postcode) {
    printf("executing function ins_record for: %s %s %s %d %c %d \n", id, firstname, lastname, birthyear, gender, postcode);

    int result = testBF(id);

    if (result == 1) {
        printf("BF may contain key: %s \n", id);

        RbtNode* node = searchRB(id);

        if (node == NULL) {
            printf("RB does not contain key \n");
        } else {
            printf("Error, duplicate \n");
            return;
        }
    } else {
        printf("BF does not contain key: %s \n", id);
    }

    // insert
    insertBF(id);


    char *copyid = strdup(id);
    char *copyfirstname = strdup(firstname);
    char *copylastname = strdup(lastname);

    RbtNode* r = insertRB(copyid, copyfirstname, copylastname, birthyear, gender, postcode);

    insertLL(r);
}

void find_key(char * key) {
    int result = testBF(key);

    if (result == 1) {
        printf("BF may contain key: %s \n", key);

        RbtNode* node = searchRB(key);

        if (node == NULL) {
            printf("# REC-WITH %s NOT-in-structs\n", key);
        } else {
            RbtData data = node->data;

            printf("# REC-IS: %s %s %s %c %d %d, vote: %d \n", key, data.firstname, data.lastname, data.gender, data.birthyear, data.postcode, node->voted);
        }
    } else {
        printf("# REC-WITH %s NOT-in-structs \n", key);
    }

}

void delete_key(char * key) {

}

void load_fileofkeys(char * filename) {
    printf("Input file loaded\n");

    FILE * fp = fopen(filename, "r");


    fp = fopen(filename, "r");

    char buffer[1000];

    while (fgets(buffer, sizeof (buffer), fp) != NULL) {
        char * firsttoken = strtok(buffer, " ");

        insertBF(firsttoken);

        // DW112135  RICHMOND ROMAN 24  M 45675
        char * a = strtok(NULL, " ");
        char * b = strtok(NULL, " ");
        char * c = strtok(NULL, " ");
        char * d = strtok(NULL, " ");
        char * e = strtok(NULL, " ");

        char *copyid = strdup(firsttoken);
        char *copyfirstname = strdup(a);
        char *copylastname = strdup(b);

        int birthyear = atoi(c);
        char gender = d[0];
        int postcode = atoi(e);

        RbtNode* r = insertRB(copyid, copyfirstname, copylastname, birthyear, gender, postcode);

        if (r != NULL) {
            r->voted = 1;
            insertLL(r);
        } else {
            free(copyid);
            free(copyfirstname);
            free(copylastname);
        }
    }

    fclose(fp);

}

int voted() {
    int c = searchVotedRBT();

    printf("# %d numberOfVotes \n", c);

    return 0;
}

void vote(char * key) {
    int result = testBF(key);

    if (result == 1) {
        printf("BF may contain key: %s \n", key);

        RbtNode* node = searchRB(key);

        if (node == NULL) {
            printf("key not found \n");
        } else {
            RbtData data = node->data;

            printf("ID: %s %s %s %c %d %d \n", data.id, data.firstname, data.lastname, data.gender, data.birthyear, data.postcode);
            if (node->voted == 0) {
                printf("Successful vote \n");
                node->voted = 1;
            } else {
                printf("already voted \n");
            }
        }
    } else {
        printf("BF does not contain key: %s \n", key);
    }
}

int voted_postcode(int postcode) {
    votePostCodeLL(postcode);
    return 0;
}

float voted_percent() {


    votedPercentLL();
    return 0;
}

void interactWithUser() {
    char buffer[5000];

    while (1) {
        printf("Type command: ");
        fgets(buffer, sizeof (buffer), stdin);

        int length = strlen(buffer);
        buffer[length - 1] = '\0';

        if (length < 3) {
            printf("unknown first token \n");
            continue;
        }

        char * firsttoken = strtok(buffer, " ");

        if (strcmp(firsttoken, "lbf") == 0) {
            printf("first token correct: lbf \n");

            char * secondtoken = strtok(NULL, " ");

            if (secondtoken == NULL) {
                printf("second token invalid \n");
            } else {
                lbf(secondtoken);
            }
        } else if (strcmp(firsttoken, "lrb") == 0) {
            printf("first token correct: lrb \n");

            char * secondtoken = strtok(NULL, " ");

            if (secondtoken == NULL) {
                printf("second token invalid \n");
            } else {
                lrb(secondtoken);
            }
        } else if (strcmp(firsttoken, "ins") == 0) {
            printf("first token correct: ins \n");

            char * secondtoken = strtok(NULL, " ");

            if (secondtoken == NULL) {
                printf("second token invalid \n");
            } else {
                char * tokenA = secondtoken;
                char * tokenB = strtok(NULL, " ");
                char * tokenC = strtok(NULL, " ");
                char * tokenD = strtok(NULL, " ");
                char * tokenE = strtok(NULL, " ");
                char * tokenF = strtok(NULL, " ");

                if (tokenA != NULL && tokenB != NULL && tokenC != NULL && tokenD != NULL && tokenE != NULL && tokenF != NULL) {
                    ins_record(tokenA, tokenB, tokenC, atoi(tokenD), tokenE[0], atoi(tokenF));
                }
            }
        } else if (strcmp(firsttoken, "find") == 0) {
            printf("first token correct: find \n");

            char * secondtoken = strtok(NULL, " ");

            if (secondtoken == NULL) {
                printf("second token invalid \n");
            } else {
                find_key(secondtoken);
            }
        } else if (strcmp(firsttoken, "delete") == 0) {
            printf("first token correct: delete \n");

            char * secondtoken = strtok(NULL, " ");

            if (secondtoken == NULL) {
                printf("second token invalid \n");
            } else {
                delete_key(secondtoken);
            }
        } else if (strcmp(firsttoken, "vote") == 0) {
            printf("first token correct: vote \n");

            char * secondtoken = strtok(NULL, " ");

            if (secondtoken == NULL) {
                printf("second token invalid \n");
            } else {
                vote(secondtoken);
            }
        } else if (strcmp(firsttoken, "load") == 0) {
            printf("first token correct: load \n");

            char * secondtoken = strtok(NULL, " ");

            if (secondtoken == NULL) {
                printf("second token invalid \n");
            } else {
                load_fileofkeys(secondtoken);
            }
        } else if (strcmp(firsttoken, "voted") == 0) {
            printf("first token correct: voted \n");

            char * secondtoken = strtok(NULL, " ");

            if (secondtoken == NULL) {
                voted();
            } else {
                voted_postcode(atoi(secondtoken));
            }
        } else if (strcmp(firsttoken, "votedperpc") == 0) {
            voted_percent();
        } else {
            printf("unknown first token \n");
        }


        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }
    }
}
