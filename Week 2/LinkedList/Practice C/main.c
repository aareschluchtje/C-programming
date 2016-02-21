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
	show();
	getch();
	return 1;
}