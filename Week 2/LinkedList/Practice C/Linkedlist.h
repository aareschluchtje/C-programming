#ifndef LLIST_DEF
#define LLIST_DEF

struct node
{
	int data;
	struct node *next;
};

void init();
int add(int data);
void show();
<<<<<<< HEAD
void Remove(int index);
=======
int remove(int index);
>>>>>>> eb0482ba6b27140f27669ae5f8d5cf584e6fc10b
#endif
