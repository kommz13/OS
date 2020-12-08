#ifndef STRIPARRAY_H
#define STRIPARRAY_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct StripData {
    pid_t pid;
    bool used;
} StripData;

StripData * initializeSA(int size);

void cleanupSA(StripData * p);

int insertSA(StripData * p, int size, pid_t pid);

int removeSA(StripData * p, int size, pid_t pid);

bool isFullSA(StripData * p, int size);

int findEmptyPositionSA(StripData * p, int size);

#endif /* STRIPARRAY_H */

