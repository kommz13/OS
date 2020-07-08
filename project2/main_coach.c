#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <fcntl.h>
#include <time.h>
#include "record.h"
#include <signal.h>

int signals_received = 0;

void f(int signum) {
    signals_received++;
}

int main(int argc, char** argv) {

    char * inputfile, * coach_up, *a, *coach, *c;
    char coach_down[8][100];
    pid_t pid_of_autonomous_process[8];
    int i, s;
    int records = 0;

    signal(SIGUSR2, f);

    srand(time(0));

    for (i = 1; i < argc; i = i + 2) {
        if (!strcmp(argv[i], "-f")) {
            inputfile = argv[i + 1];
        } else {
            if (!strcmp(argv[i], "-c")) {
                c = argv[i + 1];
            } else if (!strcmp(argv[i], "-coach")) {
                coach = argv[i + 1];
            } else if (!strcmp(argv[i], "-p")) {
                coach_up = argv[i + 1];
            } else if (!strcmp(argv[i], "-a")) {
                a = argv[i + 1];
            }
        }
    }

    int out = open(coach_up, O_WRONLY);

    FILE * fp = fopen(inputfile, "rb");

    if (!fp) {
        printf("Coach I/O error, file not found: %s \n", inputfile);
        exit(1);
    }

    while (1) {
        MyRecord temp;
        if (fread(&temp, sizeof (temp), 1, fp) <= 0) {
            break;
        } else {
            records++;
        }
    }

    fclose(fp);


    int sorters = 1 << atoi(coach);

    for (s = 0; s < sorters; s++) {
        do {
            int pipe_id = rand();
            sprintf(coach_down[s], "%d.pipe", pipe_id);
        } while (mkfifo(coach_down[s], 0666) == -1);

        printf("Coach, running with column=%s inputfile=%s  coach:%s  a: %s , pipe id: %s  \n", c, inputfile, coach, a, coach_down[s]);

        pid_of_autonomous_process[s] = fork();

        if (pid_of_autonomous_process[s] == 0) {
            char *const envp[2] = {".", NULL};

            int ncoach = atoi(coach);
            int nls, nle;

            if (ncoach == 0) {
                nls = 1;
                nle = records;
            }
            if (ncoach == 1) {
                switch (s) {
                    case 0:
                        nls = 1;
                        nle = records / 2 - 1;
                        break;
                    case 1:
                        nls = records / 2;
                        nle = records;
                        break;
                }
            }
            if (ncoach == 2) {
                switch (s) {
                    case 0:
                        nls = 1;
                        nle = records / 8 - 1;
                        break;
                    case 1:
                        nls = records / 8;
                        nle = records / 4 - 1;
                        break;
                    case 2:
                        nls = records / 4;
                        nle = records / 2 - 1;
                        break;
                    case 3:
                        nls = records / 2;
                        nle = records;
                        break;
                }
            }
            if (ncoach == 3) {
                switch (s) {
                    case 0:
                        nls = 1;
                        nle = records / 16 - 1;
                        break;
                    case 1:
                        nls = records / 16;
                        nle = records / 8 - 1;
                        break;
                    case 2:
                        nls = records / 8;
                        nle = (3 * records / 16) - 1;
                        break;
                    case 3:
                        nls = (3 * records / 16);
                        nle = records / 4 - 1;
                        break;
                    case 4:
                        nls = records / 4;
                        nle = (5 * records / 16) - 1;
                        break;
                    case 5:
                        nls = (5 * records / 16);
                        nle = records / 2;
                        break;
                    case 6:
                        nls = records / 2;
                        nle = (3 * records / 4) - 1;
                        break;
                    case 7:
                        nls = (3 * records / 4);
                        nle = records;
                        break;
                }
            }


            char ls[100];
            char le[100];

            sprintf(ls, "%d", nls);
            sprintf(le, "%d", nle);

            execle("./sorter", "sorter", "-f", inputfile, "-c", c, "-ls", ls, "-le", le, "-p", coach_down[s], "-a", a, NULL, envp);
            perror("sorter fork failed");

            return 0;
        }
    }


    double sum_runtime = 0;
    double sum_cpu_time = 0;


    double t1, t2;
    struct tms tb1, tb2;
    double ticspersec = (double) sysconf(_SC_CLK_TCK);

    t1 = (double) times(&tb1);

    char output_filename[5000];
    strcpy(output_filename, inputfile);
    strcat(output_filename, ".");
    strcat(output_filename, c);


    FILE * fp_output = fopen(output_filename, "w+");

    for (s = 0; s < sorters; s++) {
        int in = open(coach_down[s], O_RDONLY);

        double runtime = 0;
        double cpu_time = 0;

        int n;

        read(in, &runtime, sizeof (double));
        read(in, &cpu_time, sizeof (double));
        read(in, &n, sizeof (int));

        sum_runtime = sum_runtime + runtime;
        sum_cpu_time = sum_cpu_time + cpu_time;


        MyRecord record;
        for (i = 0; i < n; i++) {
            read(in, &record, sizeof (MyRecord));


            if (fp_output) {
                char rowbuffer[1000];
                sprintf(rowbuffer, "%ld %s %s  %s %d %s %s %-9.2f\n", \
		record.custid, record.LastName, record.FirstName, \
		record.Street, record.HouseID, record.City, record.postcode, \
		record.amount);
                
//                fputs(rowbuffer, fp_output);
                fwrite(rowbuffer, strlen(rowbuffer), 1, fp_output);
            }

        }

        if (waitpid(pid_of_autonomous_process[s], NULL, 0) == -1) {
            printf("Coach: Wait failed.\n");
        }

        printf("Coach: Child finished: Runtime: %lf sec (REAL time), CPU: %lf sec (CPU time).\n", runtime, cpu_time);

        close(in);

        unlink(coach_down[s]);
    }

    t2 = (double) times(&tb2);

    fclose(fp_output);

    double runtime = (t2 - t1) / ticspersec;
    double cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));


    sum_runtime = sum_runtime / sorters;
    sum_cpu_time = sum_cpu_time / sorters;

    printf("Coach %s: Avg Runtime: %lf sec (REAL time), Avg CPU: %lf sec (CPU time). signals:%d \n", coach, sum_runtime, sum_cpu_time, signals_received);

    if (out != -1) {
        write(out, &runtime, sizeof (runtime));
        write(out, &cpu_time, sizeof (cpu_time));

        close(out);
    }
    return 0;
}