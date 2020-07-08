#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h>
#include "record.h"
#include "heapsort.h"

/*pigi arxikis heapsort apo rosettacode.org*/

int greater_than(MyRecord a, MyRecord b, int c) {
    if (c == 1) {
        if (a.custid > b.custid) {
            return 1;
        }
    }
    if (c == 2) {
        if (strcmp(a.FirstName, b.FirstName) > 0) {
            return 1;
        }
    }

    if (c == 3) {
        if (strcmp(a.LastName, b.LastName) > 0) {
            return 1;
        }
    }
    if (c == 4) {
        if (strcmp(a.Street, b.Street) > 0) {
            return 1;
        }
    }
    if (c == 5) {
        if (a.HouseID > b.HouseID) {
            return 1;
        }
    }
    if (c == 6) {
        if (strcmp(a.City, b.City) > 0) {
            return 1;
        }
    }
    if (c == 7) {
        if (strcmp(a.postcode, b.postcode) > 0) {
            return 1;
        }
    }
    if (c == 8) {
        if (a.amount > b.amount) {
            return 1;
        }
    }
    return 0;
}

int max(MyRecord *a, int n, int i, int j, int k, int c) {
    int m = i;
    if (j < n && greater_than(a[j], a[m], c)) {
        m = j;
    }
    if (k < n && greater_than(a[k], a[m], c)) {
        m = k;
    }
    return m;
}

void downheap(MyRecord *a, int n, int i, int c) {
    while (1) {
        int j = max(a, n, i, 2 * i + 1, 2 * i + 2, c);
        if (j == i) {
            break;
        }
        MyRecord t = a[i];
        a[i] = a[j];
        a[j] = t;
        i = j;
    }
}

void heapsort_entry(MyRecord * records, int n, int c) {
    int i;
    for (i = (n - 2) / 2; i >= 0; i--) {
        downheap(records, n, i, c);
    }
    for (i = 0; i < n; i++) {
        MyRecord t = records[n - i - 1];
        records[n - i - 1] = records[0];
        records[0] = t;
        downheap(records, n - i - 1, 0, c);
    }
}

int heapsort_entrypoint(int argc, char** argv) {

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

    printf("Heapsort, running with column=%d inputfile=%s  [%d,%d]  \n", c, inputfile, ls, le);

    int out = open(sorting_up, O_WRONLY);


    FILE * fp = fopen(inputfile, "rb");

    if (!fp) {
        printf("Heapsort I/O error, file not found: %s \n", inputfile);
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

    heapsort_entry(records, n, c);
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


    printf("Heapsort finished: Runtime: %lf sec (REAL time), CPU: %lf sec (CPU time).\n", runtime, cpu_time);


    free(records);
    return 0;
}