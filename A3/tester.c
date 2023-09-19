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
        printf("my_calloc batman for char array: %s\n", charArray);
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

    // char *ptr = (char *)my_malloc(sizeof(char));

	// if (ptr == NULL) {
	// 	printf("Memory Error!\n");
	// } else {
	// 	*ptr = 'S';
	// 	printf("%c", *ptr);
	// }

    // int n;
	// scanf("%d", &n);

	// // Dynamically allocated array of size 10 using calloc()
	// // array elements are initialized with 0
	// // arr stores the base address of the memory block
	// char *str = (char *)my_calloc(n, sizeof(char));

	// if (str == NULL) {
	// 	printf("Memory Error!\n");
	// } else {
	// 	// initializing array with char variables
	// 	for (int i = 0; i < n; i++)	{
	// 		char ch;
	// 		scanf("%c", &ch);
	// 		*(str + i) = ch;
	// 	}
	// 	// printing array using pointer
	// 	for (int i = 0; i < n; i++) {
	// 		printf("%c", *(str + i));
	// 	}
	// }

    // dynamically allocated variable, sizeof(char) = 1 byte
	// char *ptr = (char *)my_calloc(10,sizeof(char));

	// if (ptr == NULL) {
	// 	printf("Memory Error!\n");
	// } else {
	// 	*ptr = 'asdfghjklz';
	// 	printf("%s", *ptr);
	// }

	// // deallocating memory pointed by ptr
	// free(ptr);

	// printf("\n%c ", *ptr);

	// assign NULL to avoid garbage values
	// ptr = NULL;

    return 0;
}