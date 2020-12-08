
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shared_memory.h"

// "${OUTPUT_PATH}" -d 10 -t 20 -s id

int id;
int d, t = 0, shmid;

void Step_1() {
    printf("Comptroller #%d, step: 1 \n", id);
    sleep(d);
}

void Step_2() {
    printf("Comptroller #%d, step: 2 \n", id);

    comptroller_statistics();
}

void Step_3() {
    printf("Comptroller #%d, step: 3 \n", id);
    sleep(t);
}

void Step_4() {
    printf("Comptroller #%d, step: 4 \n", id);

    comptroller_station_status();
}

int main(int argc, char** argv) {
    int i;

    id = getpid();

    for (i = 1; i < argc; i = i + 2) {
        if (!strcmp(argv[i], "-d")) {
            d = atoi(argv[i + 1]);
        } else {
            if (!strcmp(argv[i], "-t")) {
                t = atoi(argv[i + 1]);
            } else if (!strcmp(argv[i], "-s")) {
                shmid = atoi(argv[i + 1]);
            }
        }
    }

    attach(shmid);

    while (1) {
        Step_1();
        
        if (getSharedMemory()->statistics.out_buses == getSharedMemory()->configuration.buses) {
            break;
        }

        Step_2();

        Step_3();
        
        if (getSharedMemory()->statistics.out_buses == getSharedMemory()->configuration.buses) {
            break;
        }
        
        Step_4();
    }

    detach();

    printf("Comptroller: exited \n");

    return 0;
}

