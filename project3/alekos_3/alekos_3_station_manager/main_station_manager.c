
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>

#include "shared_memory.h"
#include "striparray.h"

// "${OUTPUT_PATH}" -s id

bool try_enter(BusInfo * businfo, StripData * arrayASK, StripData * arrayPEL, StripData * arrayVOR) {
    int capacity = getSharedMemory()->configuration.capacity;
    int i;
    pid_t pid = businfo->pid;

//    printf("checking for bus type: %s \n", businfo->type);
    
    if (strcmp(businfo->type, "ASK") == 0) {
        for (i = 0; i < capacity; i++) {
            if (!arrayASK[i].used) {
                arrayASK[i].pid = pid;
                arrayASK[i].used = true;

                businfo->strip = i;
                strcpy(businfo->striptype, "ASK");
                return true;
            }
        }

        for (i = 0; i < capacity; i++) {
            if (!arrayPEL[i].used) {
                arrayPEL[i].pid = pid;
                arrayPEL[i].used = true;

                businfo->strip = i;
                strcpy(businfo->striptype, "PEL");
                return true;
            }
        }
        return false;
    }

    if (strcmp(businfo->type, "PEL") == 0) {
        for (i = 0; i < capacity; i++) {
            if (!arrayPEL[i].used) {
                arrayPEL[i].pid = pid;
                arrayPEL[i].used = true;

                businfo->strip = i;
                strcpy(businfo->striptype, "PEL");
                return true;
            }
        }
         return false;
    }

    if (strcmp(businfo->type, "VOR") == 0) {
        for (i = 0; i < capacity; i++) {
            if (!arrayVOR[i].used) {
                arrayVOR[i].pid = pid;
                arrayVOR[i].used = true;

                businfo->strip = i;
                strcpy(businfo->striptype, "VOR");
                return true;
            }
        }
        for (i = 0; i < capacity; i++) {
            if (!arrayPEL[i].used) {
                arrayPEL[i].pid = pid;
                arrayPEL[i].used = true;

                businfo->strip = i;
                strcpy(businfo->striptype, "PEL");
                return true;
            }
        }
        return false;
    }
    
    printf("bus info critical :'%s', pid: %d \n",businfo->type, pid);
    printf("##################### oime #####################\n");
    exit(1);
//    return false;
}

bool try_exit(BusInfo * businfo, StripData * arrayASK, StripData * arrayPEL, StripData * arrayVOR) {
    int capacity = getSharedMemory()->configuration.capacity;
    int i;
    pid_t pid = businfo->pid;

    if (strcmp(businfo->type, "ASK") == 0) {
        for (i = 0; i < capacity; i++) {
            if (arrayASK[i].used && arrayASK[i].pid == pid) {
                arrayASK[i].used = false;
                return true;
            }
        }

        for (i = 0; i < capacity; i++) {
            if (arrayPEL[i].used && arrayPEL[i].pid == pid) {
                arrayPEL[i].used = false;
                return true;
            }
        }
        return false;
    }

    if (strcmp(businfo->type, "PEL") == 0) {
        for (i = 0; i < capacity; i++) {
            if (arrayPEL[i].used && arrayPEL[i].pid == pid) {
                arrayPEL[i].used = false;
                return true;
            }
        }
    }

    if (strcmp(businfo->type, "VOR") == 0) {
        for (i = 0; i < capacity; i++) {
            if (arrayVOR[i].used && arrayVOR[i].pid == pid) {
                arrayVOR[i].used = false;
                return true;
            }
        }
        for (i = 0; i < capacity; i++) {
            if (arrayPEL[i].used && arrayPEL[i].pid == pid) {
                arrayPEL[i].used = false;
                return true;
            }
        }
        return false;
    }

    return false;
}

int main(int argc, char** argv) {
    int i, shmid;

    for (i = 1; i < argc; i = i + 2) {
        if (!strcmp(argv[i], "-s")) {
            shmid = atoi(argv[i + 1]);
        }
    }

    printf("Station manager: started \n");

    attach(shmid);

    printf("Station manager: attached \n");

    int progress = 0;

    int capacity = getSharedMemory()->configuration.capacity;

    StripData * arrayASK = initializeSA(capacity);
    StripData * arrayPEL = initializeSA(capacity);
    StripData * arrayVOR = initializeSA(capacity);
//    StripData * man = initializeSA(capacity);

    BusInfo * waitingInEntrance = 0;

    int next_pid = 0;

    while (1) {
        printf("Station manager: Waiting ... \n");
        QueueItem item = manager_consume();


        if (item.action == ACTION_ENTRY_REQUEST) {
            printf("Station manager: consumed: PID:%d Action:%d / ACTION_ENTRY_REQUEST \n", item.pid, item.action);

            // assign ID
            assignIDtoBus(item.pid);

            // notify ID was assigned
            sem_post(&getSharedMemory()->semaphores.bus_inc);

            // wait for ID to be retrieved
            sem_wait(&getSharedMemory()->semaphores.bus_id);

            BusInfo * businfo = getBusInfo(item.pid);

            // if can enter   
            if (try_enter(businfo, arrayASK, arrayPEL, arrayVOR)) {
                printf("permission granted \n");
                sem_post(&getSharedMemory()->semaphores.bus_inc);
            } else {
                // else add it to waiting queue
                printf("permission denied for  pid: %d \n", item.pid);
                waitingInEntrance = businfo;
            }

            next_pid++;
        } else if (item.action == ACTION_ENTRY) {
            printf("Station manager: consumed: PID:%d Action:%d / ACTION_ENTRY \n", item.pid, item.action);
            sem_wait(&getSharedMemory()->semaphores.ledger);
            getSharedMemory()->statistics.parked++;
            getSharedMemory()->statistics.available_spots--;
            getSharedMemory()->statistics.in_buses++;

            if (strcmp(getBusInfo(item.pid)->type, "PEL") == 0) {
                getSharedMemory()->statistics.in_buses_pel++;
                getSharedMemory()->statistics.parked_pel--;

            }
            if (strcmp(getBusInfo(item.pid)->type, "VOR") == 0) {
                getSharedMemory()->statistics.in_buses_vor++;
                getSharedMemory()->statistics.parked_vor--;
            }
            if (strcmp(getBusInfo(item.pid)->type, "ASK") == 0) {
                getSharedMemory()->statistics.in_buses_ask++;
                getSharedMemory()->statistics.parked_ask--;

            }

            getBusInfo(item.pid)->arrival_time = time(0);

            sem_post(&getSharedMemory()->semaphores.ledger);
        } else if (item.action == ACTION_EXIT_REQUEST) {
            printf("Station manager: consumed: PID:%d Action:%d / ACTION_EXIT_REQUEST \n", item.pid, item.action);

            BusInfo * businfo = getBusInfo(item.pid);

            try_exit(businfo, arrayASK, arrayPEL, arrayVOR);

            if (waitingInEntrance != NULL) {
                BusInfo * firstbus = waitingInEntrance;

                if (try_enter(firstbus, arrayASK, arrayPEL, arrayVOR)) {
                    sem_post(&getSharedMemory()->semaphores.bus_inc);
                    
                    waitingInEntrance = NULL;
                }
            }

        } else if (item.action == ACTION_EXIT) {
            printf("Station manager: consumed: PID:%d Action:%d / ACTION_EXIT \n", item.pid, item.action);
            int pid = item.pid;

            double delay = getBusInfo(pid)->delaytime;


            getBusInfo(item.pid)->departure_time = time(0);

            time_t service_time = getBusInfo(pid)->departure_time - getBusInfo(pid)->arrival_time;

            getBusInfo(item.pid)->service_time = getBusInfo(item.pid)->departure_time - getBusInfo(item.pid)->arrival_time;


            sem_wait(&getSharedMemory()->semaphores.ledger);
            getSharedMemory()->statistics.mean_waiting_time = getSharedMemory()->statistics.mean_waiting_time + delay;
            getSharedMemory()->statistics.out_buses++;
            getSharedMemory()->statistics.parked--;
            getSharedMemory()->statistics.total_buses++;
            getSharedMemory()->statistics.total_passengers += getBusInfo(pid)->numbers_of_inc_passengers;
            getSharedMemory()->statistics.total_passengers += getBusInfo(pid)->numbers_of_out_passengers;
            getSharedMemory()->statistics.available_spots++;
            getSharedMemory()->statistics.mean_service_time += service_time;

            if (strcmp(getBusInfo(pid)->type, "PEL") == 0) {
                getSharedMemory()->statistics.mean_waiting_time_pel = getSharedMemory()->statistics.mean_waiting_time_pel + delay;
                getSharedMemory()->statistics.out_buses_pel++;
                getSharedMemory()->statistics.parked_pel--;

            }
            if (strcmp(getBusInfo(pid)->type, "VOR") == 0) {
                getSharedMemory()->statistics.mean_waiting_time_vor = getSharedMemory()->statistics.mean_waiting_time_vor + delay;
                getSharedMemory()->statistics.out_buses_vor++;
                getSharedMemory()->statistics.parked_vor--;
            }
            if (strcmp(getBusInfo(pid)->type, "ASK") == 0) {
                getSharedMemory()->statistics.mean_waiting_time_ask = getSharedMemory()->statistics.mean_waiting_time_ask + delay;
                getSharedMemory()->statistics.out_buses_ask++;
                getSharedMemory()->statistics.parked_ask--;

            }



            sem_post(&getSharedMemory()->semaphores.ledger);


            progress++;
        } else {
            printf("_{_?{}:>{}:>{:}>{:}>>}:{>}:{>}:>{}:>{}:>{}:");
            printf("_{_?{}:>{}:>{:}>{:}>>}:{>}:{>}:>{}:>{}:>{}:");
            printf("_{_?{}:>{}:>{:}>{:}>>}:{>}:{>}:>{}:>{}:>{}:");
            printf("_{_?{}:>{}:>{:}>{:}>>}:{>}:{>}:>{}:>{}:>{}:");
            exit(1);
        }


        if (progress == getSharedMemory()->configuration.buses) {
            break;
        }
    }

    detach();


    cleanupSA(arrayASK);
    cleanupSA(arrayPEL);
    cleanupSA(arrayVOR);

    printf("Station manager: exited \n");

    return 0;
}

