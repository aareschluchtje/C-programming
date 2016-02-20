#include <stddef.h>
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

#endif // !LLIST_DEF

int add(int data)
{
	struct node *pn = (struct node*)malloc(sizeof(struct node));

	if (NULL == pn)
	{
		printf("Out of memory...");
	}
	else
	{
		if (NULL == pHead)
		{
			pn->data = data;
			pn->next = NULL;
			pHead = pn;
		}
		else
		{
			pn->data = data;
			pn->next = pHead;
			pHead = pn;
		}
	}
}