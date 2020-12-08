
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mystation.h"

// "${OUTPUT_PATH}" -l config

int main(int argc, char** argv) {
    char * configfile = NULL;
    int i;

    for (i = 1; i < argc; i = i + 2) {
        if (!strcmp(argv[i], "-l")) {
            configfile = argv[i + 1];
        }
    }

    printf("configfile     = %s \n", configfile);

    initialize(configfile);

    createStructures();
    
    createProcesses();
    
    
    cleanUpProcesses();
    
    cleanUpStructures();
    
    return 0;
}

