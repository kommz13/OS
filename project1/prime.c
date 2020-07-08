#include "prime.h"

int is_prime(int n) {
    int i;
    
    if (n % 2 == 0 | n % 3 == 0 | n % 5 == 0 | n % 7 == 0) {
        return 0;
    }
    
    for (i=3;i<n/2;i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    
    return 1;
}