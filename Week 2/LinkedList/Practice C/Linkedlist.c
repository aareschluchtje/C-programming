#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef LLIST_DEF
#define LLIST_DEF

static struct node *pHead = NULL;

struct node
{
	int data;
	struct node *next;
};

void init()
{
	pHead = NULL;

}

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

void show()
{
	struct node *p = pHead;
	int nr = 0;

	for (; NULL != p->next; p = p->next)
	{
		printf("node nr: %d heeft data [%d]\n", nr++, p->data);
	}
}
