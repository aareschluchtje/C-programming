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
int remove(int index);
void clear();
int nrItems();
int excist(int data);
void deepCopy();
void showCopy();
#endif
