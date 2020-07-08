#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "quicksort.h"
#include "record.h"

/* pigi arxikis quicksort apo  geeksforgeeks.org */

// swap

void swap(MyRecord* a, MyRecord* b) {
    MyRecord temp = *a;
    *a = *b;
    *b = temp;
}

/* This function takes last element as pivot, places 
   the pivot element at its correct position in sorted 
    array, and places all smaller (smaller than pivot) 
   to left of pivot and all greater elements to right 
   of pivot */
int partition(MyRecord arr[], int low, int high, int c) {
    MyRecord pivot = arr[high]; // pivot 
    int i = (low - 1); // Index of smaller element 

    for (int j = low; j <= high - 1; j++) {
        // If current element is smaller than the pivot 

        if (c == 1) {
            if (arr[j].custid < pivot.custid) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        if (c == 2) {
            if (strcmp(arr[j].FirstName, pivot.FirstName) < 0) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }

        if (c == 3) {
            if (strcmp(arr[j].LastName, pivot.LastName) < 0) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        if (c == 4) {
            if (strcmp(arr[j].Street, pivot.Street) < 0) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        if (c == 5) {
            if (arr[j].HouseID < pivot.HouseID) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        if (c == 6) {
            if (strcmp(arr[j].City, pivot.City) < 0) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        if (c == 7) {
            if (strcmp(arr[j].postcode, pivot.postcode) < 0) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        if (c == 8) {
            if (arr[j].amount < pivot.amount) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }

    }
    swap(&arr[i + 1], &arr[high]);

    return (i + 1);
}

/* The main function that implements QuickSort 
 arr[] --> Array to be sorted, 
  low  --> Starting index, 
  high  --> Ending index */
void quickSort(MyRecord arr[], int low, int high, int c) {
    if (low < high) {
        /* pi is partitioning index, arr[p] is now 
           at right place */
        int pi = partition(arr, low, high, c);

        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, low, pi - 1, c);
        quickSort(arr, pi + 1, high, c);
    }
}

void quicksort_entry(MyRecord * records, int n, int c) {

    quickSort(records, 0, n - 1, c);
}