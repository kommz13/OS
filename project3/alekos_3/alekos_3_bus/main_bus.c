
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/times.h> 

#include "shared_memory.h"

char * t;
pid_t pid;
int i, n, c, p, m, shmid;
double ticspersec;

// "${OUTPUT_PATH}" -t PEL -n 10 -c 50 -p 5 -m 3 -s id




// =================================================
//              1. Arrival and entry request
// =================================================

double Step_1() {
    double t1;
    struct tms tb1;

    printf("Bus #%d, type=%s, step: 1 \n", pid, t);

    t1 = (double) times(&tb1);

    sem_wait(&getSharedMemory()->semaphores.bus_entrance);

    QueueItem item;
    item.action = ACTION_ENTRY_REQUEST;
    item.pid = getpid();

    manager_produce(item);

    return t1;
}

// =================================================
//              2. Wait for slot assignment
// =================================================

void Step_2() {
    printf("Bus #%d, type=%s, step: 2 \n", pid, t);

    // wait for id
    sem_wait(&getSharedMemory()->semaphores.bus_inc);

    getBusInfo(pid)->numbers_of_inc_passengers = n;
    getBusInfo(pid)->numbers_of_out_passengers = 0;
    strcpy(getBusInfo(pid)->type, t);

    // notify id was received
    sem_post(&getSharedMemory()->semaphores.bus_id);
    
    // wait for entrace permission
    sem_wait(&getSharedMemory()->semaphores.bus_inc);

    // release entrance for other buses
    sem_post(&getSharedMemory()->semaphores.bus_entrance);
}

// =================================================
//              3. Manoeuver
// =================================================

double Step_3(double t1) {
    struct tms tb2;
    double t2 = (double) times(&tb2);

    printf("Bus #%d, type=%s, step: 3 \n", pid, t);

    double run_time = (t2 - t1) / ticspersec;


    sem_wait(&getSharedMemory()->semaphores.maneuver_in);
    printf("lock manauever in \n");
    if (m > 0) {
        sleep(1 + rand() % m);
    }
    printf("unlock manauever in \n");
    sem_post(&getSharedMemory()->semaphores.maneuver_in);

    QueueItem item;
    item.action = ACTION_ENTRY;
    item.pid = getpid();

    manager_produce(item);

    return run_time;
}

// =================================================
//              4. Update ledger
// =================================================

void Step_4() {
    printf("Bus #%d, type=%s, step: 4 \n", pid, t);


    sem_wait(&getSharedMemory()->semaphores.ledger);
    getSharedMemory()->statistics.passengers_inc = getSharedMemory()->statistics.passengers_inc + n;
    sem_post(&getSharedMemory()->semaphores.ledger);


    //    bus_increase_passengers(n); // increase inc
}

// =================================================
//              5. Park, unload and load passengers
// =================================================

void Step_5() {
    printf("Bus #%d, type=%s, step: 5, strip name: %s strip no:%d \n", pid, t, getBusInfo(pid)->striptype, getBusInfo(pid)->strip);

    if (c > 0) {
        getBusInfo(pid)->numbers_of_out_passengers = 1 + rand() % c;
    }

    sem_wait(&getSharedMemory()->semaphores.ledger);
    getSharedMemory()->statistics.passengers_out = getSharedMemory()->statistics.passengers_out + getBusInfo(pid)->numbers_of_out_passengers;
    sem_post(&getSharedMemory()->semaphores.ledger);

    if (p > 0) {
        sleep(1 + rand() % p);
    }
}

// =================================================
//              6. Exit request
// =================================================

double Step_6() {
    double t1;
    struct tms tb1;

    t1 = (double) times(&tb1);

    printf("Bus #%d, type=%s, step: 6 \n", pid, t);

    QueueItem item;
    item.action = ACTION_EXIT_REQUEST;
    item.pid = getpid();

    manager_produce(item);

    return t1;
}

// =================================================
//              7. Wait for confirmation
// =================================================

double Step_7(double t1) {
    struct tms tb2;
    double t2 = (double) times(&tb2);

    double run_time = (t2 - t1) / ticspersec;


    printf("Bus #%d, type=%s, step: 7 \n", pid, t);

    return run_time;
}

// =================================================
//              8. departure
// =================================================

void Step_8(double runtime) {
    printf("Bus #%d, type=%s, step: 8 \n", pid, t);


    sem_wait(&getSharedMemory()->semaphores.maneuver_out);
    printf("lock manauever out \n");
    if (m > 0) {
        sleep(1 + rand() % m);
    }
    printf("unlock manauever out \n");
    sem_post(&getSharedMemory()->semaphores.maneuver_out);

    getBusInfo(pid)->delaytime = runtime;

    QueueItem item;
    item.action = ACTION_EXIT;
    item.pid = getpid();

    manager_produce(item);
}

void Step_9() {
    printf("Bus #%d, type=%s, step: 9 \n", pid, t);
}

int main(int argc, char** argv) {
    pid = getpid();

    srand(getpid());

    ticspersec = (double) sysconf(_SC_CLK_TCK);

    for (i = 1; i < argc; i = i + 2) {
        if (!strcmp(argv[i], "-t")) {
            t = argv[i + 1];
        } else {
            if (!strcmp(argv[i], "-n")) {
                n = atoi(argv[i + 1]);
            } else if (!strcmp(argv[i], "-c")) {
                c = atoi(argv[i + 1]);
            } else if (!strcmp(argv[i], "-p")) {
                p = atoi(argv[i + 1]);
            } else if (!strcmp(argv[i], "-m")) {
                m = atoi(argv[i + 1]);
            } else if (!strcmp(argv[i], "-s")) {
                shmid = atoi(argv[i + 1]);
            }
        }
    }

    attach(shmid);

    printf("Bus #%d, running with type=%s n=%d, c=%d, p=%d, m=%d, s=%d\n", pid, t, n, c, p, m, shmid);

    double t1 = Step_1();

    Step_2();

    double runtime1 = Step_3(t1);

    Step_4();

    Step_5();

    double t2 = Step_6();

    double runtime2 = Step_7(t2);

    double runtime = runtime1 + runtime2;

    Step_8(runtime);
    
    Step_9();

    detach();

    printf("Bus, exited with type=%s n=%d, c=%d, p=%d, m=%d, s=%d\n", t, n, c, p, m, shmid);


    return 0;
}

