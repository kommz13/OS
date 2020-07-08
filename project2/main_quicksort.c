#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h>
#include "record.h"
#include "quicksort.h"

int quicksort_entrypoint(int argc, char** argv) {

    char * inputfile, * sorting_up;
    int c, ls, le;
    int i;

    for (i = 1; i < argc; i = i + 2) {
        if (!strcmp(argv[i], "-f")) {
            inputfile = argv[i + 1];
        } else {
            if (!strcmp(argv[i], "-c")) {
                c = atoi(argv[i + 1]);
            } else if (!strcmp(argv[i], "-ls")) {
                ls = atoi(argv[i + 1]);
            } else if (!strcmp(argv[i], "-le")) {
                le = atoi(argv[i + 1]);
            } else if (!strcmp(argv[i], "-p")) {
                sorting_up = argv[i + 1];
            }
        }
    }

    printf("Quicksort, running with column=%d inputfile=%s  [%d,%d]  \n", c, inputfile, ls, le);

    int out = open(sorting_up, O_WRONLY);


    FILE * fp = fopen(inputfile, "rb");

    if (!fp) {
        printf("Quicksort I/O error, file not found: %s \n", inputfile);
        exit(1);
    }

    MyRecord * records = malloc(sizeof (MyRecord)*(le - ls + 1));

    fseek(fp, (ls - 1)*(sizeof (MyRecord)), SEEK_SET);

    for (i = ls; i <= le; i++) {
        fread(&records[i - ls], sizeof (records[i]), 1, fp);
    }


    double t1, t2;
    struct tms tb1, tb2;
    double ticspersec = (double) sysconf(_SC_CLK_TCK);

    t1 = (double) times(&tb1);

    // --------------------------------
    int n = le - ls + 1;
    
    quicksort_entry(records, n, c);
    // --------------------------------

    t2 = (double) times(&tb2); 

    double runtime = (t2 - t1) / ticspersec;
    double cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));

    fclose(fp);

    write(out, &runtime, sizeof (double));
    write(out, &cpu_time, sizeof (double));
    write(out, &n, sizeof (int));

    for (i = 0; i < (le - ls + 1); i++) {
        write(out, &records[i], sizeof (MyRecord));
    }

    close(out);


    printf("Quicksort finished: Runtime: %lf sec (REAL time), CPU: %lf sec (CPU time).\n", runtime, cpu_time);

    
    free(records);
    return 0;
}