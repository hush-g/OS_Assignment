#include <stdio.h>
#include <stdlib.h>
#include "my_mmu_boilerplate.h"

int main() {
    // Test Case 1: my_malloc for int
    int* intPtr = (int*)my_malloc(sizeof(int));
    if (intPtr != NULL) {
        *intPtr = 42;
        printf("my_malloc for int: %d\n", *intPtr);
        info();
        my_free(intPtr);
        info();
    } else {
        printf("my_malloc failed\n");
    }

    // Test Case 2: my_calloc for char array
    char* charArray = (char*)my_calloc(20, sizeof(char));
    if (charArray != NULL) {
        for (int i = 0; i < 20; i++) {
            charArray[i] = 'A' + i;
        }
        printf("my_calloc for char array: %s\n", charArray);
        info();
        my_free(charArray);
        info();
    } else {
        printf("my_calloc failed\n");
    }

    // Test Case 3: my_malloc for double
    double* doublePtr = (double*)my_malloc(sizeof(double));
    if (doublePtr != NULL) {
        *doublePtr = 3.14;
        printf("my_malloc for double: %f\n", *doublePtr);
        info();
        my_free(doublePtr);
        info();
    } else {
        printf("my_malloc failed\n");
    }

    return 0;
}