#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "record.h"
#include "quicksort.h"
#include "heapsort.h"
#include <signal.h>

int main(int argc, char** argv) {
    char * inputfile, * sorting_up, *a, *ls, *le, *c;
    int i;

    srand(time(0));

    for (i = 1; i < argc; i = i + 2) {
        if (!strcmp(argv[i], "-f")) {
            inputfile = argv[i + 1];
        } else {
            if (!strcmp(argv[i], "-c")) {
                c = argv[i + 1];
            } else if (!strcmp(argv[i], "-ls")) {
                ls = argv[i + 1];
                le = argv[i + 1];
            } else if (!strcmp(argv[i], "-p")) {
                sorting_up = argv[i + 1];
            } else if (!strcmp(argv[i], "-a")) {
                a = argv[i + 1];
            }
        }
    }

    printf("Sorter, running with column=%s inputfile=%s  [%s,%s] a: %s , pipe id: %s  \n", c, inputfile, ls, le, a, sorting_up);

    if (strcmp(a, "h") == 0) {
        int ret = heapsort_entrypoint(argc, argv);

        pid_t ppid = getppid();
        kill(ppid, SIGUSR2);

        return ret;
    }
    if (strcmp(a, "q") == 0) {
        int ret = quicksort_entrypoint(argc, argv);

        pid_t ppid = getppid();
        kill(ppid, SIGUSR2);

        return ret;
    }

    return -1;
}