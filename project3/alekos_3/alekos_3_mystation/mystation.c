#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#include "mystation.h"
#include "shared_memory.h"

#define LOGFILE "log.txt"
#define LOGLINE 200

char linebuffer[LOGLINE];

char * _configfile = NULL;

pid_t pid_of_comptroller;
pid_t pid_of_station_manager;
pid_t * pids_of_buses;

int nisides = 3;
int nisida_capacity = 10;
int max_epivates = 50;
int max_manoeuver = 10;
int max_parking = 10;
int current_status_time = 5;
int statistics_time = 10;
int buses = 50;

int shmid = 99;

StaticSharedMemory *mem;
BusInfo * businfo;

void initialize(char * configfile) {
    _configfile = configfile;

    FILE * fp = fopen(configfile, "r");

    int k;
    if (fp) {
        char token1[1000], token2[1000];

        while ((k = (fscanf(fp, "%s %s", token1, token2))) == 2) {
            if (strcmp(token1, "nisides") == 0) {
                nisides = atoi(token2);
            }
            if (strcmp(token1, "nisida_capacity") == 0) {
                nisida_capacity = atoi(token2);
            }
            if (strcmp(token1, "max_epivates") == 0) {
                max_epivates = atoi(token2);
            }
            if (strcmp(token1, "max_parking") == 0) {
                max_parking = atoi(token2);
            }
            if (strcmp(token1, "max_manoeuver") == 0) {
                max_manoeuver = atoi(token2);
            }
            if (strcmp(token1, "current_status_time") == 0) {
                current_status_time = atoi(token2);
            }
            if (strcmp(token1, "statistics_time") == 0) {
                statistics_time = atoi(token2);
            }
            if (strcmp(token1, "buses") == 0) {
                buses = atoi(token2);
            }
        }
    } else {
        printf("Config file not found, using default params \n");
    }

    srand(time(0));

    printf("App initialized with buses: %d \n", buses);

    remove(LOGFILE);

    snprintf(linebuffer, sizeof (linebuffer), "App initialized with buses: %d \n", buses);

    log_write(LOGFILE, linebuffer, 0);
}

void createStructures() {
    int i, retval;
    int memory_bytes = sizeof (StaticSharedMemory) + sizeof (BusInfo) * buses;

    /* Make shared memory segment */
    shmid = shmget(IPC_PRIVATE, memory_bytes, 0666);
    if (shmid == -1) {
        perror("Creation");
        exit(1);
    } else {
        printf("Allocated Shared Memory with ID: %d\n", (int) shmid);
    }

    /* Attach the segment */
    mem = (StaticSharedMemory *) shmat(shmid, (void*) 0, 0);
    businfo = (BusInfo *) (((char*) mem) + sizeof (StaticSharedMemory));

    mem->configuration.buses = buses;
    mem->configuration.capacity = nisida_capacity;
    mem->configuration.strips = nisides;

    mem->statistics.available_spots = nisida_capacity*nisides;
    mem->statistics.in_buses = 0;
    mem->statistics.passengers_out = 0;

    for (i = 0; i < buses; i++) {
        businfo[i].pid = 0;
        businfo[i].arrival_time = 0;
        businfo[i].departure_time = 0;
        businfo[i].service_time = 0;
        businfo[i].delaytime = 0;
        businfo[i].numbers_of_inc_passengers = 0;
        businfo[i].numbers_of_out_passengers = 0;
        businfo[i].status = 0;
        businfo[i].strip = 0;
    }

    mem->queueItem.action = 0;
    mem->queueItem.pid = 0;

    retval = sem_init(&mem->semaphores.maneuver_in, 1, 1);
    if (retval != 0) {
        perror("Couldn't initialize bus_inc.");
        exit(3);
    }

    retval = sem_init(&mem->semaphores.maneuver_out, 1, 1);
    if (retval != 0) {
        perror("Couldn't initialize bus_inc.");
        exit(3);
    }


    retval = sem_init(&mem->semaphores.bus_inc, 1, 0);
    if (retval != 0) {
        perror("Couldn't initialize bus_inc.");
        exit(3);
    }
    
    retval = sem_init(&mem->semaphores.bus_id, 1, 0);
    if (retval != 0) {
        perror("Couldn't initialize bus_inc.");
        exit(3);
    }

    retval = sem_init(&mem->semaphores.bus_out, 1, 0);
    if (retval != 0) {
        perror("Couldn't initialize bus_out.");
        exit(3);
    }

    retval = sem_init(&mem->semaphores.ledger, 1, 1);
    if (retval != 0) {
        perror("Couldn't initialize ledger.");
        exit(3);
    }

    retval = sem_init(&mem->semaphores.sme, 1, 1);
    if (retval != 0) {
        perror("Couldn't initialize sme.");
        exit(3);
    }

    retval = sem_init(&mem->semaphores.smf, 1, 0);
    if (retval != 0) {
        perror("Couldn't initialize smf.");
        exit(3);
    }

    retval = sem_init(&mem->semaphores.bus_entrance, 1, 1);
    if (retval != 0) {
        perror("Couldn't initialize bus_entrance.");
        exit(3);
    }

    retval = sem_init(&mem->semaphores.log, 1, 1);
    if (retval != 0) {
        perror("Couldn't initialize bus_entrance.");
        exit(3);
    }
}

void createProcesses() {
    int c;


    printf("App creating processes. \n");

    printf("App creating station_manager. \n");

    pid_of_comptroller = fork();

    if (pid_of_comptroller == 0) {
        char *const envp[2] = {".", NULL};

        char d[100];
        char t[100];
        char s[100];

        sprintf(d, "%d", current_status_time);
        sprintf(t, "%d", statistics_time);
        sprintf(s, "%d", shmid);

        printf("./comptroller %s %s %s %s %s %s \n", "-d", d, "-t", t, "-s", s);

        execle("./comptroller", "comptroller", "-d", d, "-t", t, "-s", s, NULL, envp);

        perror("comptroller fork failed");

        exit(1);
    }

    printf("App creating comptroller. \n");

    pid_of_station_manager = fork();

    if (pid_of_station_manager == 0) {
        char *const envp[2] = {".", NULL};

        char s[100];

        sprintf(s, "%d", shmid);

        printf("./station_manager %s %s \n", "-s", s);

        execle("./station_manager", "station_manager", "-s", s, NULL, envp);

        perror("station_manager fork failed");

        exit(0);
    }

    printf("App creating buses. \n");

    pids_of_buses = malloc(sizeof (pid_t) * buses);

    for (c = 0; c < buses; c++) {
        pids_of_buses[c] = fork();

        if (pids_of_buses[c] == 0) {
            char *const envp[2] = {".", NULL};

            char bustype[100];
            char n[100];
            char cc[100];
            char p[100];
            char m[100];
            char s[100];

            srand(getpid());

            switch (rand() % 3) {
                case 0:
                    sprintf(bustype, "%s", "ASK");
                    break;
                case 1:
                    sprintf(bustype, "%s", "PEL");
                    break;
                case 2:
                    sprintf(bustype, "%s", "VOR");
                    break;
            }

            sprintf(n, "%d", rand() % max_epivates);
            sprintf(cc, "%d", max_epivates);
            
            if (max_parking > 0) {
                sprintf(p, "%d", rand() % max_parking);
            } else {
                sprintf(p, "%d", 0);
            }
            
            if (max_manoeuver > 0) {
                sprintf(m, "%d", rand() % max_manoeuver);
            } else {
                sprintf(m, "%d", 0);
            }
            sprintf(s, "%d", shmid);

            printf("./bus %s %s %s %s %s %s %s %s %s %s %s %s\n", "-t", bustype, "-n", n, "-c", cc, "-p", p, "-m", m, "-s", s);

            execle("./bus", "bus", "-t", bustype, "-n", n, "-c", cc, "-p", p, "-m", m, "-s", s, NULL, envp);

            perror("bus fork failed");

            exit(0);
        }
    }

}

void cleanUpStructures() {
    printf("Structures cleaned\n");

    sem_destroy(&mem->semaphores.maneuver_in);
    sem_destroy(&mem->semaphores.maneuver_out);
    sem_destroy(&mem->semaphores.bus_inc);
    sem_destroy(&mem->semaphores.bus_out);
    sem_destroy(&mem->semaphores.ledger);
    sem_destroy(&mem->semaphores.sme);
    sem_destroy(&mem->semaphores.smf);
    sem_destroy(&mem->semaphores.bus_id);
    sem_destroy(&mem->semaphores.bus_entrance);
    sem_destroy(&mem->semaphores.log);

    shmdt((void*) mem);
}

void cleanUpProcesses() {
    int c;

    for (c = 0; c < buses; c++) {
        waitpid(pids_of_buses[c], NULL, 0);

        printf("mystation: *********************************** Bus finished: %d *************************************\n ", pids_of_buses[c]);
    }

    waitpid(pid_of_comptroller, NULL, 0);

    printf("mystation: pid_of_comptroller finished.\n");

    waitpid(pid_of_station_manager, NULL, 0);

    printf("mystation: pid_of_station_manager finished.\n");

    free(pids_of_buses);

    //
    printf("--------------------\n");
    printf("mystation results \n");
    printf("--------------------\n");

    printf("nisides: %d \n", nisides);
    printf("nisida_capacity: %d \n", nisida_capacity);
    printf("max_epivates: %d \n", max_epivates);
    printf("max_parking: %d \n", max_parking);
    printf("current_status_time: %d \n", current_status_time);
    printf("statistics_time: %d \n", statistics_time);
    printf("buses: %d \n", buses);


    printf("-------------------------------------------------------\n");
    printf("                    Statitics \n");
    printf("-------------------------------------------------------\n");
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


    printf("-------------------------------------------------------\n");
    printf("                    Businfo \n");
    printf("-------------------------------------------------------\n");

    BusInfo * ptr = getBusInfoPtr();

    for (c = 0; c < buses; c++) {
        printf("%20s: %d \n", "pid", ptr[c].pid);
        
        char buffer[26];
        struct tm* tm_info;
        time_t temp = ptr[c].arrival_time;

        tm_info = localtime(&temp);

        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        
        printf("%20s: %d (%s) \n", "arrival_time", (int) ptr[c].arrival_time, buffer);

        temp = ptr[c].departure_time;

        tm_info = localtime(&temp);
        
         strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        
        printf("%20s: %d (%s)", "departure_time", (int) ptr[c].departure_time, buffer);


        printf("%20s: %s \n", "type", ptr[c].type);
        printf("%20s: %d \n", "strip", ptr[c].strip);
        printf("%20s: %d \n", "numbers_of_inc_passengers", ptr[c].numbers_of_inc_passengers);
        printf("%20s: %d \n", "numbers_of_out_passengers", ptr[c].numbers_of_out_passengers);
        printf("%20s: %d \n", "status", ptr[c].status);
        printf("%20s: %lf \n", "delaytime", ptr[c].delaytime);

    }

    printf("Processes cleaned\n");
}
