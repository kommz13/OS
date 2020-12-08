
#include "striparray.h"

StripData * initializeSA(int size) {
    int i;

    StripData * data = (StripData *) malloc(sizeof (StripData) * size);

    for (i = 0; i < size; i++) {
        data[i].pid = -1;
        data[i].used = false;
    }

    return data;
}

void cleanupSA(StripData * p) {
    free(p);
}

int insertSA(StripData * data, int size, pid_t pid) {
    int i;

    for (i = 0; i < size; i++) {
        if (data[i].used == false) {
            data[i].pid = pid;
            data[i].used = true;
            return i;
        }
    }
    return -1;
}

int removeSA(StripData * data, int size, pid_t pid) {
    int i;
    for (i = 0; i < size; i++) {
        if (data[i].used == true && data[i].pid == pid) {
            data[i].pid = -1;
            data[i].used = false;
            return i;
        }
    }
    return -1;
}

bool isFullSA(StripData * data, int size) {
    int i;
    
    for (i = 0; i < size; i++) {
        if (data[i].used == false) {
            return false;
        }
    }
    return true;
}

int findEmptyPositionSA(StripData * data, int size) {
    int i;
    
    for (i = 0; i < size; i++) {
        if (data[i].used == false) {
            return i;
        }
    }
    return -1;
}