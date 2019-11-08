#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

void initializeBF(int bits);

void cleanupBF();

void resetBF();

void insertBF(char * key);

int testBF(char * key);

#endif