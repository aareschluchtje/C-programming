#include <stdio.h>
#include "Linkedlist.h"
#include <stdlib.h>
#include <math.h>

char * toArray(int number);

int main()
{
	printf(toArray(13413));
	
	int idx;

	init();

	for (idx = 0; idx < 10; idx++)
	{
		add(idx);
	}
/*	printf("Remove opdracht :\n");
	remove(4);
	clear();
	printf("Element in de list : %d \n", excist(4));
	printf("Count linklist: %d \n", nrItems());
	printf("original:\n");*/
	printf("Show opdracht :\n");
	show();
	printf("Deepcopy opdracht :\n");
	deepCopy();
	printf("copy:\n");
	showCopy();
	getchar();
	return 1;
}

char * toArray(int number)
{
	int n = log10(number) + 1;
	int i;
	char *numberArray = calloc(n, sizeof(char));
	for (i = 0; i < n; ++i, number /= 10)
	{
		numberArray[i] = number % 10;
	}
	return numberArray;
}