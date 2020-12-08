
#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "shared_memory.h"


StaticSharedMemory *mem;
BusInfo * businfo;

StaticSharedMemory * getSharedMemory() {
    return mem;
}

BusInfo * getBusInfo(pid_t pid) {
    int i;

    for (i = 0; i < mem->configuration.buses; i++) {
        if (businfo[i].pid == pid) {
            return &businfo[i];
        }
    }

    printf("_{_?{}:>{}:>{:}>{:}>>}:{>}:{>}:>{}:>{}:>{}:");

    return NULL;
}

BusInfo * getBusInfoPtr() {
    return &businfo[0];
}

void assignIDtoBus(pid_t pid) {
    int i;

    for (i = 0; i < mem->configuration.buses; i++) {
        if (businfo[i].pid == 0) {
            businfo[i].pid = pid;
            return;
        }
    }

    printf("_{_?{}:>{}:>{:}>{:}>>}:{>}:{>}:>{}:>{}:>{}:");
}

void attach(int shmid) {
    mem = (StaticSharedMemory *) shmat(shmid, (void*) 0, 0);

    if (mem == (StaticSharedMemory *) - 1) {
        perror("Attach failed \n");
        exit(1);
    }
    businfo = (BusInfo *) (((char*) mem) + sizeof (StaticSharedMemory));
}

void detach() {
    shmdt((void*) mem);
}

QueueItem manager_consume() {
    QueueItem item;

    sem_wait(&mem->semaphores.smf);

    item.action = mem->queueItem.action;
    item.pid = mem->queueItem.pid;

    sem_post(&mem->semaphores.sme);

    return item;
}

void manager_produce(QueueItem item) {
    sem_wait(&mem->semaphores.sme);

    mem->queueItem.action = item.action;
    mem->queueItem.pid = item.pid;

    sem_post(&mem->semaphores.smf);
}

void comptroller_statistics() {
    sem_wait(&mem->semaphores.ledger);

    printf("******************************************\n");
    printf("Parked:  %d \n", mem->statistics.parked);
    printf("Total passngers:  %d \n", mem->statistics.total_passengers);

    int denom;
    int denom_pel;
    int denom_ask;
    int denom_vor;

    if (getSharedMemory()->statistics.total_buses == 0) {
        denom = 1;
    } else {
        denom = getSharedMemory()->statistics.total_buses;
    }

    if (getSharedMemory()->statistics.in_buses_pel == 0) {
        denom_pel = 1;
    } else {
        denom_pel = getSharedMemory()->statistics.in_buses_pel;
    }

    if (getSharedMemory()->statistics.in_buses_ask == 0) {
        denom_ask = 1;
    } else {
        denom_ask = getSharedMemory()->statistics.in_buses_ask;
    }

    if (getSharedMemory()->statistics.in_buses_vor == 0) {
        denom_vor = 1;
    } else {
        denom_vor = getSharedMemory()->statistics.in_buses_vor;
    }


    printf("%20s: %d \n", "parked", getSharedMemory()->statistics.parked);
    printf("%20s: %d \n", "available_spots", getSharedMemory()->statistics.available_spots);
    printf("%20s: %d \n", "passengers_inc", getSharedMemory()->statistics.passengers_inc);
    printf("%20s: %d \n", "passengers_out", getSharedMemory()->statistics.passengers_out);
    printf("%20s: %f \n", "mean_service_time", getSharedMemory()->statistics.mean_service_time / denom);
    printf("%20s: %f \n", "mean_waiting_time", getSharedMemory()->statistics.mean_waiting_time / denom);
    printf("%20s: %d \n", "in_buses", getSharedMemory()->statistics.in_buses);
    printf("%20s: %d \n", "out_buses", getSharedMemory()->statistics.out_buses);
    printf("%20s: %f \n", "mean_waiting_time_pel", getSharedMemory()->statistics.mean_waiting_time_pel / denom_pel);
    printf("%20s: %f \n", "mean_waiting_time_vor", getSharedMemory()->statistics.mean_waiting_time_vor / denom_vor);
    printf("%20s: %f \n", "mean_waiting_time_ask", getSharedMemory()->statistics.mean_waiting_time_ask / denom_ask);
    printf("%20s: %d \n", "in_buses_pel", getSharedMemory()->statistics.in_buses_pel);
    printf("%20s: %d \n", "out_buses_pel", getSharedMemory()->statistics.out_buses_pel);
    printf("%20s: %d \n", "in_buses_vor", getSharedMemory()->statistics.in_buses_vor);
    printf("%20s: %d \n", "out_buses_vor", getSharedMemory()->statistics.out_buses_vor);
    printf("%20s: %d \n", "in_buses_ask", getSharedMemory()->statistics.in_buses_ask);
    printf("%20s: %d \n", "out_buses_ask", getSharedMemory()->statistics.out_buses_ask);
    printf("%20s: %d \n", "total_buses", getSharedMemory()->statistics.total_buses);
    printf("%20s: %d \n", "total_passengers", getSharedMemory()->statistics.total_passengers);


    printf("******************************************\n");

    sem_post(&mem->semaphores.ledger);
}

void comptroller_station_status() {
    sem_wait(&mem->semaphores.ledger);
  
    printf("******************************************\n");
    printf("Parked:  %d \n", mem->statistics.parked);

    printf("%20s: %d \n", "parked", getSharedMemory()->statistics.parked);
    printf("%20s: %d \n", "available_spots", getSharedMemory()->statistics.available_spots);
    printf("%20s: %d \n", "passengers_inc", getSharedMemory()->statistics.passengers_inc);
    printf("%20s: %d \n", "passengers_out", getSharedMemory()->statistics.passengers_out);


    printf("******************************************\n");


    sem_post(&mem->semaphores.ledger);
}

void log_write(const char * filename, const char * content, int withlock) {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[10000];

    if (withlock) {
        sem_wait(&mem->semaphores.log);
    }

    FILE * fp = fopen(filename, "a+");

    time(&rawtime);

    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%m/%d/%Y %H:%M:%S %Z:", timeinfo);
    strcat(buffer, content);

    fprintf(fp, "%s\n", buffer);

    fclose(fp);

    if (withlock) {
        sem_post(&mem->semaphores.log);
    }
}