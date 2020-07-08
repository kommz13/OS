#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coordinator.h"

int main(int argc, char** argv) {
    char * inputfile = NULL;
    int i, j = 0, k=0;

    char * sortings[4] = {NULL, NULL, NULL, NULL};      //sortings algos
    int sorting_columns[4] = {0, 0, 0, 0};              // sorting columns from file

    for (i = 1; i < argc; i = i + 2) {
        if (!strcmp(argv[i], "-f")) {
            inputfile = argv[i + 1];
        } else {
            if (!strcmp(argv[i], "-h")) {
                if (j >= 4) {
                    printf("too many arguments \n");
                    return -1;
                }
                sortings[j] = argv[i];
                sorting_columns[j] = atoi(argv[i + 1]);
                j++;
            } else if (!strcmp(argv[i], "-q")) {
                if (j >= 4) {
                    printf("too many arguments \n");
                    return -1;
                }
                sortings[j] = argv[i];
                sorting_columns[j] = atoi(argv[i + 1]);
                j++;
            }
        }
    }

    for (i = 0; i < j; i++) {
        for (k = 0; k < j; k++) {
            if (i == k) {
                continue;
            }

            if (sorting_columns[i] == sorting_columns[k]) {
                printf("duplicate column numbers \n");
                return -1;
            }
        }

    }

    printf("inputfile     = %s \n", inputfile);

    for (i = 0; i < 4; i++) {
        if (sortings[i] != NULL) {
            if (strcmp(sortings[i], "-h") == 0) {
                printf("Sorting algorithm %d: Heapsort on column %d \n", i, sorting_columns[i]);
            }
            if (strcmp(sortings[i], "-q") == 0) {
                printf("Sorting algorithm %d: Quicksort on column %d \n", i, sorting_columns[i]);
            }
        } else {
            break;
        }
    }


    initialize(inputfile, sortings, sorting_columns);

    createCoaches();

    cleanUpCoaches();

    return 0;
}