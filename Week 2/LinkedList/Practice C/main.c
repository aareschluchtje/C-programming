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
<<<<<<< HEAD

	Remove(4);
=======
	remove(3);
>>>>>>> eb0482ba6b27140f27669ae5f8d5cf584e6fc10b
	show();
	getchar();
	return 1;
}
