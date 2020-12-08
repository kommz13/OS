
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "functions.h"
#include "list.h"
#include "interactWithUser.h"

// -bs <BLOCKSIZE>
// -fns <FILENAMESIZE>:
// -cfs <MAXFILESIZE>
// -mdfn <MAXDIRECTORYFILENUMBER>

int main(int argc, char** argv) {
    int i;

    int blocksize = 0;
    int filenamesize = 0;
    int maxfilesize = 0;
    int maxdirectoryfilenumber = 0;

    if (argc != 9) {
        printf("missing arguments \n");
        return 0;
    }

    for (i = 1; i < argc; i = i + 2) {
        if (!strcmp(argv[i], "-bs")) {
            blocksize = atoi(argv[i + 1]);
        } else {
            if (!strcmp(argv[i], "-fns")) {
                filenamesize = atoi(argv[i + 1]);
            } else if (!strcmp(argv[i], "-cfs")) {
                maxfilesize = atoi(argv[i + 1]);
            } else if (!strcmp(argv[i], "-mdfn")) {
                maxdirectoryfilenumber = atoi(argv[i + 1]);
            } else {
                printf("unknown argument \n");
                return 0;
            }
        }
    }


    printf("blocksize                  = %d \n", blocksize);
    printf("filenamesize               = %d \n", filenamesize);
    printf("maxfilesize                = %d \n", maxfilesize);
    printf("maxdirectoryfilenumber     = %d \n", maxdirectoryfilenumber);

        
    unlink("boo.cfs");
    
    interactWithUser();

    return 0;
}

