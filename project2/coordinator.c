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
#include "coordinator.h"

char * _inputfile = NULL;

char ** _sortings = NULL;
int * _sorting_columns = NULL;
pid_t pids[4];
int _coaches = 0;

pid_t pid_of_autonomous_process[4];           //coaches pid array
char coordinator_down[4][100];                //coordinator to coach pipe names array

void initialize(char * inputfile, char * sortings[], int * sorting_columns) {
    int i;

    _inputfile = inputfile;
    _sortings = sortings;
    _sorting_columns = sorting_columns;


    for (i = 0; i < 4; i++) {
        if (sortings[i] != NULL) {
            _coaches++;
        }
    }

    srand(time(0));

    printf("App initialized with coaches: %d \n", _coaches);
}

void createCoaches() {
    int c;

    printf("Coaches created \n");

    printf("App creating coaches: %d \n", _coaches);

    for (c = 0; c < _coaches; c++) {
        do {
            int pipe_id = rand();
            sprintf(coordinator_down[c], "%d.pipe", pipe_id);
        } while (mkfifo(coordinator_down[c], 0666) == -1);       //dimourgia pipe apo coordinator pros coach me tis times apo to pipe_id

        pid_of_autonomous_process[c] = fork();

        if (pid_of_autonomous_process[c] == 0) {
            char *const envp[2] = {".", NULL};

            char coachtype[100];
            char algorithm[100];
            char column[100];

            sprintf(coachtype, "%d", c);
            
            if (strcmp(_sortings[c], "-h")==0) {
                strcpy(algorithm, "h");
            } else {
                strcpy(algorithm, "q");
            }
            
            sprintf(column, "%d", _sorting_columns[c]);
            
            printf("./coach %s %s %s %s %s %s %s %s %s %s\n","-f", _inputfile, "-c", column, "-coach", coachtype, "-p", coordinator_down[c], "-a", algorithm);

            execle("./coach", "coach", "-f", _inputfile, "-c", column, "-coach", coachtype, "-p", coordinator_down[c], "-a", algorithm, NULL, envp);
            perror("coach fork failed");

            exit(0);
        }
    }


}

void cleanUpCoaches() {
    int c;
    double sum_runtime = 0;
    double sum_cpu_time = 0;


    double t1, t2;
    struct tms tb1, tb2;
    double ticspersec = (double) sysconf(_SC_CLK_TCK);

    t1 = (double) times(&tb1);

    // --------------------------------

    for (c = 0; c < _coaches; c++) {
        int in = open(coordinator_down[c], O_RDONLY);                  //o fd anoigei to pipe

        double runtime = 0;
        double cpu_time = 0;

        read(in, &runtime, sizeof (double));
        read(in, &cpu_time, sizeof (double));

        sum_runtime = sum_runtime + runtime;
        sum_cpu_time = sum_cpu_time + cpu_time;

        waitpid(pid_of_autonomous_process[c], NULL, 0);               //wait gia to paidi

        printf("Coordinator: Child finished: Runtime: %lf sec (REAL time), CPU: %lf sec (CPU time).\n", runtime, cpu_time);

        close(in);                                                   //kleinoume to pipe

        unlink(coordinator_down[c]);                                 //sbinoume to pipe
    }

    // --------------------------------

    t2 = (double) times(&tb2);

    double runtime = (t2 - t1) / ticspersec;
    double cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));



    sum_runtime = sum_runtime / _coaches;
    sum_cpu_time = sum_cpu_time / _coaches;

    printf("--------------------\n");
    printf("coordinator results \n");
    printf("--------------------\n");

    printf("Average coach runtime : %lf \n", sum_runtime);
    printf("Average coach CPU time: %lf \n", sum_cpu_time);
    printf("Turn around runtime: %lf \n", runtime);
    printf("Turn around cputime: %lf \n", cpu_time);
    
    printf("Coaches cleaned\n");
}