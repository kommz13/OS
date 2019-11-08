
#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

int main(int argc, char** argv) {
    char * inputfile;
    char * outputfile;
    int numofupdates;
    
    if (argc != 7){
        printf("invalid arguments \n");
        return 1;
    }

    // TODO: validate arguments  (exit on error)
    //          parse arguments in any order
    //
    inputfile = argv[2];
    outputfile = argv[4];
    numofupdates = atoi(argv[6]);
    
    printf("inputfile     = %s \n", inputfile);
    printf("outputfile    = %s \n", outputfile);
    printf("numofupdates  = %d \n", numofupdates);
    
    initialize(outputfile, numofupdates);
    
    createDataStructures();
    
    loadfile(inputfile);
    
    interactWithUser();
    
    cleanUpDataStructures();
    
    return 0;
}

