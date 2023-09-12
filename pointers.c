

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void Create2DHeapArray(int rows, int cols, int ***pppOutArr)
{
	int **ppArr = (int**)malloc(rows * sizeof(int*));
	for(int i = 0; i < rows; i++){
		ppArr[i] = (int*)malloc(cols * sizeof(int));
	}
	
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			ppArr[i][j] = rand() % 100 + 1;
		}	
	}
	*pppOutArr = ppArr;
}


int main() {

	int arr2d[3][4]; // stack array (8 MB of stack memory)

	int a = 10; // stack variable
	printf ("a is : %d\n", a);
	
	int *pA = &a; // pointer pA stores the address of variable a
	int **ppA = &pA; // pointer-to-pointer ppA stores the address of variable pointer pA
	int ***pppA = &ppA; // pointer-to-pointer-to-pointer pppA stores the address of variable pointer ppA
	
	// Dereferencing all the way to access variable a
	*(*(*pppA)) = 30; 
	printf ("a now is : %d\n", a);
	
	
	int **pp2DArr = NULL;
	Create2DHeapArray(3, 4, &pp2DArr);
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 4; j++){
			printf("%d\t", pp2DArr[i][j]);
		}
		printf("\n");
	}
	
	for(int i = 0; i < 3; i++){
		free(pp2DArr[i]);
	}
	free(pp2DArr);
	

    return 0;
}

