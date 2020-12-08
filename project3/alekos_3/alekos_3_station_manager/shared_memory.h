
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <unistd.h>
#include <semaphore.h>

#define ACTION_ENTRY_REQUEST 1
#define ACTION_ENTRY 2
#define ACTION_EXIT_REQUEST 3
#define ACTION_EXIT 4

typedef struct Statistics {
    int parked;
    int parked_pel;
    int parked_ask;
    int parked_vor;
    int available_spots;
    int passengers_inc;
    int passengers_out;

    float mean_service_time; // in + out
    float mean_waiting_time; // in + out
    int in_buses;
    int out_buses;

    float mean_waiting_time_pel; // in + out
    float mean_waiting_time_vor; // in + out
    float mean_waiting_time_ask; // in + out
    int in_buses_pel;
    int out_buses_pel;
    int in_buses_vor;
    int out_buses_vor;
    int in_buses_ask;
    int out_buses_ask;
    
    int total_buses;
    int total_passengers;
} Statistics;

typedef struct BusInfo {
    pid_t pid;
    time_t arrival_time;
    time_t departure_time;
    time_t service_time;
    char type[4];
    int strip;
    char striptype[4];
    int numbers_of_inc_passengers;
    int numbers_of_out_passengers;
    int status;
    double delaytime;
} BusInfo;

typedef struct Semaphores {
    sem_t bus_inc;
    sem_t bus_out;
    sem_t bus_id;
    sem_t smf;
    sem_t sme;
    sem_t ledger;
    sem_t maneuver_in;
    sem_t maneuver_out;
    sem_t bus_entrance;
    sem_t log;
} Semaphores;

typedef struct QueueItem {
    pid_t pid;
    int action;
} QueueItem;

typedef struct Configuration {
    int strips;
    int capacity;
    int buses;
} Configuration;

typedef struct StaticSharedMemory {
    Configuration configuration;
    Statistics statistics;
    Semaphores semaphores;
    QueueItem queueItem;
} StaticSharedMemory;

StaticSharedMemory * getSharedMemory();

BusInfo * getBusInfo(pid_t pid);

BusInfo * getBusInfoPtr();

void assignIDtoBus(pid_t pid);

void attach(int id);

void detach();

QueueItem manager_consume();

void manager_produce(QueueItem item);


// -----------------------------------------

void comptroller_statistics();

void comptroller_station_status();

// -----------------------------------------

void log_write(const char * filename, const char * content, int withlock);


#endif /* SHARED_MEMORY_H */

