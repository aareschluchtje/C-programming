#include <stdio.h>
#include "Linkedlist.h"

int main()
{
	int idx;

	init();

	for (idx = 0; idx < 10; idx++)
	{
		add(idx);
	}
	//remove(4);
	//clear();
	printf("Element in de list : %d \n", excist(4));
	printf("Count linklist: %d \n", nrItems());
	show();
	getchar();
	return 1;
}
