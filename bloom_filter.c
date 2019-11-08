
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bloom_filter.h"

unsigned char * filter = NULL;
int bytes;
int filter_bits;

// https://www.partow.net/programming/hashfunctions/#BloomFilters

unsigned int hashA(const char* str) {
    int sum = 0;
    int i;
    for (i = 0; i < strlen(str); i++) {
        sum = sum + str[i];
    }

    return sum % 11;
}

unsigned int hashB(const char* str) {
    int sum = 0;
    int i;
    for (i = 0; i < strlen(str); i++) {
        sum = sum + str[i];
    }

    return sum % 17;
}

unsigned int hashC(const char* str) {
    int sum = 0;
    int i;
    for (i = 0; i < strlen(str); i++) {
        sum = sum + str[i];
    }

    return sum % 3001;
}

void initializeBF(int bits) {
    int i =0;
    filter_bits = bits;
    bytes = bits / 8;

    if (bytes * 8 != bits) {
        bytes++;
    }

    printf("BF initial size: %d bytes, %d bits \n", bytes, bytes * 8);

    filter = malloc(sizeof (unsigned char)*bytes);

    for (i = 0; i < bytes; i++) {
        filter[i] = 0;
    }
}

void cleanupBF() {
    free(filter);
}

void resetBF() {
    int i;

    for (i = 0; i < bytes; i++) {
        filter[i] = 0;
    }
}

void insertBF(char * key) {
    unsigned int pos1 = hashA(key) % filter_bits;
    unsigned int pos2 = hashB(key) % filter_bits;
    unsigned int pos3 = hashC(key) % filter_bits;

    unsigned int arrayPos1 = pos1 / 8;
    unsigned int arrayPos2 = pos2 / 8;
    unsigned int arrayPos3 = pos3 / 8;

    unsigned int bytePos1 = pos1 - arrayPos1 * 8;
    unsigned int bytePos2 = pos2 - arrayPos2 * 8;
    unsigned int bytePos3 = pos3 - arrayPos3 * 8;


    filter[arrayPos1] = filter[arrayPos1] | (1 << (7 - bytePos1));
    filter[arrayPos2] = filter[arrayPos2] | (1 << (7 - bytePos2));
    filter[arrayPos3] = filter[arrayPos3] | (1 << (7 - bytePos3));
}

int testBF(char * key) { // return 1 iif all bits are 1 as position1, 2, 3
    unsigned int pos1 = hashA(key) % filter_bits;
    unsigned int pos2 = hashB(key) % filter_bits;
    unsigned int pos3 = hashC(key) % filter_bits;

    unsigned int arrayPos1 = pos1 / 8;
    unsigned int arrayPos2 = pos2 / 8;
    unsigned int arrayPos3 = pos3 / 8;

    unsigned int bytePos1 = pos1 - arrayPos1 * 8;
    unsigned int bytePos2 = pos2 - arrayPos2 * 8;
    unsigned int bytePos3 = pos3 - arrayPos3 * 8;

    unsigned int bit1 = filter[arrayPos1] & (1 << (7 - bytePos1));

    if (bit1 == 0) {
        return 0;
    }

    unsigned int bit2 = filter[arrayPos2] & (1 << (7 - bytePos2));

    if (bit2 == 0) {
        return 0;
    }

    // printf("arrayPos3 = %d \n", arrayPos3);

    unsigned int bit3 = filter[arrayPos3] & (1 << (7 - bytePos3));

    if (bit3 == 0) {
        return 0;
    }

    return 1;
}